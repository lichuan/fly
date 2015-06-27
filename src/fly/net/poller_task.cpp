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
#include <unistd.h>
#include "fly/base/logger.hpp"
#include "fly/net/poller_task.hpp"
#include "fly/net/parser_task.hpp"
#include "fly/net/holder.hpp"

namespace fly {
namespace net {

Poller_Task::Poller_Task(uint64 seq) : Loop_Task(seq)
{
    m_fd = epoll_create(2);

    if(m_fd < 0)
    {
        LOG_FATAL("epoll_create failed in Poller_Task::Poller_Task");
    }
}

void Poller_Task::register_connection(std::shared_ptr<Connection> connection)
{
    struct epoll_event event;
    connection->m_poller_task = this;
    event.data.ptr = connection.get();
    event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
    int32 ret = epoll_ctl(m_fd, EPOLL_CTL_ADD, connection->m_fd, &event);
    
    if(ret < 0)
    {
        LOG_FATAL("epoll_ctl failed in Poller_Task::register_connection");
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
        uint32 event = events[i].events;
        
        if(event & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
        {
            epoll_ctl(m_fd, EPOLL_CTL_DEL, connection->m_fd, NULL);
            close(connection->m_fd);
            connection->m_holder->connection_be_closed(connection->shared_from_this());
        }
        else if(event & EPOLLIN)
        {
            while(true)
            {
                auto REQ_SIZE = 4096;
                Message_Block_Queue &recv_queue = connection->m_recv_msg_queue;
                std::unique_ptr<Message_Block> message_block(new Message_Block(REQ_SIZE));
                int32 num = read(connection->m_fd, message_block->read_ptr(), REQ_SIZE);
                
                if(num < 0)
                {
                    if(errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        break;
                    }
                }
                
                if(num <= 0)
                {
                    epoll_ctl(m_fd, EPOLL_CTL_DEL, connection->m_fd, NULL);
                    close(connection->m_fd);
                    connection->m_holder->connection_be_closed(connection->shared_from_this());

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
        else if(event & EPOLLOUT)
        {
            Message_Block_Queue &send_queue = connection->m_send_msg_queue;

            while(Message_Block *message_block = send_queue.pop())
            {
                int32 message_length = message_block->length();
                int32 num = write(m_fd, message_block->read_ptr(), message_length);
                
                if(num < 0)
                {
                    if(errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        break;
                    }
                }

                if(num == 0)
                {
                    LOG_FATAL("write return 0, it's impossible, in Poller_Task::run_in_loop");
                }
                
                if(num <= 0)
                {
                    epoll_ctl(m_fd, EPOLL_CTL_DEL, connection->m_fd, NULL);
                    close(connection->m_fd);
                    connection->m_holder->connection_be_closed(connection->shared_from_this());

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
    }
}

}
}
