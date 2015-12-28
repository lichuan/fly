/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                    _______    _                                     *
 *                   (  ____ \  ( \     |\     /|                      * 
 *                   | (    \/  | (     ( \   / )                      *
 *                   | (__      | |      \ (_) /                       *
 *                   |  __)     | |       \   /                        *
 *                   | (        | |        ) (                         *
 *                   | )        | (____/\  | |                         *
 *                   |/         (_______/  \_/                         *
 *                                                                     *
 *                                                                     *
 *     fly is an awesome c++11 network library.                        *
 *                                                                     *
 *   @author: lichuan                                                  *
 *   @qq: 308831759                                                    *
 *   @email: lichuan@lichuan.me                                        *
 *   @github: https://github.com/lichuan/fly                           *
 *   @date: 2015-06-24 20:43:56                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include "fly/base/logger.hpp"
#include "fly/net/poller_task.hpp"
#include "fly/net/parser_task.hpp"

namespace fly {
namespace net {

Poller_Task::Poller_Task(uint64 seq) : Loop_Task(seq)
{
    m_fd = epoll_create1(0);
    
    if(m_fd < 0)
    {
        LOG_FATAL("epoll_create1 failed in Poller_Task::Poller_Task");
    }
    
    m_close_event_fd = eventfd(0, 0);
    
    if(m_close_event_fd < 0)
    {
        LOG_FATAL("close event eventfd failed in Poller_Task::Poller_Task");
    }

    m_write_event_fd = eventfd(0, 0);

    if(m_write_event_fd < 0)
    {
        LOG_FATAL("write event eventfd failed in Poller_Task::Poller_Task");
    }

    struct epoll_event event;
    m_close_udata.reset(new Connection(m_close_event_fd, Addr("close_event", 0)));
    event.data.ptr = m_close_udata.get();
    event.events = EPOLLIN;
    int32 ret = epoll_ctl(m_fd, EPOLL_CTL_ADD, m_close_event_fd, &event);
    
    if(ret < 0)
    {
        LOG_FATAL("close event epoll_ctl failed in Poller_Task::Poller_Task");
    }
    
    m_write_udata.reset(new Connection(m_write_event_fd, Addr("write_event", 0)));
    event.data.ptr = m_write_udata.get();
    ret = epoll_ctl(m_fd, EPOLL_CTL_ADD, m_write_event_fd, &event);
    
    if(ret < 0)
    {
        LOG_FATAL("write event epoll_ctl failed in Poller_Task::Poller_Task");
    }
}

void Poller_Task::register_connection(std::shared_ptr<Connection> connection)
{
    struct epoll_event event;
    event.data.ptr = connection.get();
    event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
    int32 ret = epoll_ctl(m_fd, EPOLL_CTL_ADD, connection->m_fd, &event);
    
    if(ret < 0)
    {
        LOG_FATAL("epoll_ctl failed in Poller_Task::register_connection");

        return;
    }
    
    connection->m_poller_task = this;
    connection->m_self = connection;
    connection->m_init_cb(connection);
}

void Poller_Task::close_connection(std::shared_ptr<Connection> connection)
{
    m_close_queue.push(connection);
    uint64 data = 1;
    int32 num = write(m_close_event_fd, &data, sizeof(uint64));

    if(num != sizeof(uint64))
    {
        LOG_FATAL("write m_close_event_fd failed in Poller_Task::close_connection");
    }
}

void Poller_Task::write_connection(std::shared_ptr<Connection> connection)
{
    m_write_queue.push(connection);
    uint64 data = 1;
    int32 num = write(m_write_event_fd, &data, sizeof(uint64));
    
    if(num != sizeof(uint64))
    {
        LOG_FATAL("write m_write_event_fd failed in Poller_Task::write_connection");
    }
}

void Poller_Task::do_write(std::shared_ptr<Connection> connection)
{
    int32 fd = connection->m_fd;
    Message_Chunk_Queue &send_queue = connection->m_send_msg_queue;
    
    while(Message_Chunk *message_chunk = send_queue.pop())
    {
        int32 message_length = message_chunk->length();
        int32 num = write(fd, message_chunk->read_ptr(), message_length);

        if(num < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                send_queue.push_front(message_chunk);
                
                break;
            }
        }

        if(num == 0)
        {
            LOG_FATAL("write return 0, it's impossible, in Poller_Task::run_in_loop");
        }
        
        if(num <= 0)
        {
            epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, NULL);
            close(fd);
            connection->m_self.reset();
            connection->m_closed.store(true, std::memory_order_relaxed);
            connection->m_be_closed_cb(connection);
            
            break;
        }
                
        if(num < message_length)
        {
            message_chunk->read_ptr(num);
            send_queue.push_front(message_chunk);

            break;
        }
        else
        {
            delete message_chunk;
        }
    }
}

void Poller_Task::do_write()
{
    uint64 data = 0;
    int32 num = read(m_write_event_fd, &data, sizeof(uint64));

    if(num != sizeof(uint64))
    {
        LOG_FATAL("read m_write_event_fd failed in Poller_Task::do_write");

        return;
    }
    
    while(std::shared_ptr<Connection> connection = m_write_queue.pop())
    {
        if(!connection->m_closed.load(std::memory_order_relaxed))
        {
            do_write(connection);
        }
    }
}

void Poller_Task::do_close()
{
    uint64 data = 0;
    int32 num = read(m_close_event_fd, &data, sizeof(uint64));
    
    if(num != sizeof(uint64))
    {
        LOG_FATAL("read m_close_event_fd failed in Poller_Task::do_write");
        
        return;
    }
    
    while(std::shared_ptr<Connection> connection = m_close_queue.pop())
    {
        if(!connection->m_closed.load(std::memory_order_relaxed))
        {
            int32 fd = connection->m_fd;
            epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, NULL);
            close(fd);
            connection->m_self.reset();
            connection->m_closed.store(true, std::memory_order_relaxed);
            connection->m_close_cb(connection);
        }
    }
}

void Poller_Task::run_in_loop()
{
    struct epoll_event events[2048];
    int32 fd_num = epoll_wait(m_fd, events, 2048, -1);
    
    if(fd_num < 0)
    {
        LOG_FATAL("epoll_wait failed in Poller_Task::run_in_loop");

        return;
    }
    
    for(auto i = 0; i < fd_num; ++i)
    {
        Connection *connection = static_cast<Connection*>(events[i].data.ptr);
        int32 fd = connection->m_fd;
        uint32 event = events[i].events;
        
        if(event & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
        {
            epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, NULL);
            close(fd);
            connection->m_closed.store(true, std::memory_order_relaxed);
            connection->m_be_closed_cb(connection->shared_from_this());
            connection->m_self.reset();
        }
        else if(event & EPOLLIN)
        {
            if(fd == m_close_event_fd)
            {
                do_close();
            }
            else if(fd == m_write_event_fd)
            {
                do_write();
            }
            else
            {
                while(true)
                {
                    auto REQ_SIZE = 4096;
                    Message_Chunk_Queue &recv_queue = connection->m_recv_msg_queue;
                    std::unique_ptr<Message_Chunk> message_chunk(new Message_Chunk(REQ_SIZE));
                    int32 num = read(fd, message_chunk->read_ptr(), REQ_SIZE);

                    if(num < 0)
                    {
                        if(errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            break;
                        }
                    }
                
                    if(num <= 0)
                    {
                        epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, NULL);
                        close(fd);
                        connection->m_closed.store(true, std::memory_order_relaxed);
                        connection->m_be_closed_cb(connection->shared_from_this());
                        connection->m_self.reset();

                        break;
                    }
                    
                    message_chunk->write_ptr(num);
                    recv_queue.push(message_chunk.release());
                    connection->m_parser_task->push_connection(connection->shared_from_this());
                    
                    if(num < REQ_SIZE)
                    {
                        break;
                    }
                }
            }
        }
        else if(event & EPOLLOUT)
        {
            do_write(connection->shared_from_this());
        }
    }
}

}
}
