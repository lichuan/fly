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
 *   @email: 308831759@qq.com                                          *
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
#include "fly/net/holder.hpp"

namespace fly {
namespace net {

Poller_Task::Poller_Task(uint64 seq) : Loop_Task(seq)
{
    m_fd = epoll_create(4); //set 4 temporarily for test case
    
    if(m_fd < 0)
    {
        LOG_FATAL("epoll_create failed in Poller_Task::Poller_Task");
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
    event.events = EPOLLIN;
    int32 ret = epoll_ctl(m_fd, EPOLL_CTL_ADD, m_close_event_fd, &event);

    if(ret < 0)
    {
        LOG_FATAL("close event epoll_ctl failed in Poller_Task::Poller_Task");
    }

    ret = epoll_ctl(m_fd, EPOLL_CTL_ADD, m_write_event_fd, &event);
    
    if(ret < 0)
    {
        LOG_FATAL("write event epoll_ctl failed in Poller_Task::Poller_Task");
    }
}

void Poller_Task::register_connection(std::shared_ptr<Connection> connection)
{
    struct epoll_event event;
    connection->m_poller_task = this;
    connection->m_holder->Holder::init_connection(connection);
    event.data.ptr = connection.get();
    event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
    int32 ret = epoll_ctl(m_fd, EPOLL_CTL_ADD, connection->m_fd, &event);
    
    if(ret < 0)
    {
        LOG_FATAL("epoll_ctl failed in Poller_Task::register_connection");

        return;
    }
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
    Message_Block_Queue &send_queue = connection->m_send_msg_queue;

    while(Message_Block *message_block = send_queue.pop())
    {
        int32 message_length = message_block->length();
        int32 num = write(m_fd, message_block->read_ptr(), message_length);
        
        if(num < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                send_queue.push_front(message_block);
                
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
            connection->m_holder->Holder::connection_be_closed(connection->shared_from_this());

            break;
        }
                
        if(num < message_length)
        {
            message_block->read_ptr(num);
            send_queue.push_front(message_block);

            break;
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
        do_write(connection);
    }
}

void Poller_Task::do_close()
{
    while(std::shared_ptr<Connection> connection = m_close_queue.pop())
    {
        int32 fd = connection->m_fd;
        epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
        connection->m_holder->Holder::close_connection(connection->shared_from_this());
    }
}

void Poller_Task::run_in_loop()
{
    struct epoll_event events[1024];
    int32 fd_num = epoll_wait(m_fd, events, 1024, -1);

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
            connection->m_holder->Holder::connection_be_closed(connection->shared_from_this());
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
                    Message_Block_Queue &recv_queue = connection->m_recv_msg_queue;
                    std::unique_ptr<Message_Block> message_block(new Message_Block(REQ_SIZE));
                    int32 num = read(fd, message_block->read_ptr(), REQ_SIZE);
                
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
                        connection->m_holder->Holder::connection_be_closed(connection->shared_from_this());

                        break;
                    }
                
                    message_block->write_ptr(num);
                    recv_queue.push(message_block.release());
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
