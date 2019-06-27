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
#include "fly/net/poller_executor.hpp"

namespace fly {
namespace net {

template<typename T>
Poller_Executor<T>::Poller_Executor()
{
    m_fd = epoll_create1(0);
    
    if(m_fd < 0)
    {
        LOG_FATAL("epoll_create1 failed in Poller_Executor::Poller_Executor %s", strerror(errno));
        return;
    }
    
    m_close_event_fd = eventfd(0, 0);
    
    if(m_close_event_fd < 0)
    {
        LOG_FATAL("close event eventfd failed in Poller_Executor::Poller_Executor");
        return; 
    }

    m_write_event_fd = eventfd(0, 0);

    if(m_write_event_fd < 0)
    {
        LOG_FATAL("write event eventfd failed in Poller_Executor::Poller_Executor");
        return; 
    }

    m_stop_event_fd = eventfd(0, 0);

    if(m_stop_event_fd < 0)
    {
        LOG_FATAL("stop event eventfd failed in Poller_Executor::Poller_Executor");
        return; 
    }

    struct epoll_event event;
    m_close_udata.reset(new Connection<T>(m_close_event_fd, Addr("close_event", 0)));
    event.data.ptr = m_close_udata.get();
    event.events = EPOLLIN;
    int32 ret = epoll_ctl(m_fd, EPOLL_CTL_ADD, m_close_event_fd, &event);
    
    if(ret < 0)
    {
        LOG_FATAL("close event epoll_ctl failed in Poller_Executor::Poller_Executor");
        return; 
    }

    m_write_udata.reset(new Connection<T>(m_write_event_fd, Addr("write_event", 0)));
    event.data.ptr = m_write_udata.get();
    ret = epoll_ctl(m_fd, EPOLL_CTL_ADD, m_write_event_fd, &event);
    
    if(ret < 0)
    {
        LOG_FATAL("write event epoll_ctl failed in Poller_Executor::Poller_Executor");
        return; 
    }
    
    m_stop_udata.reset(new Connection<T>(m_stop_event_fd, Addr("stop_event", 0)));
    event.data.ptr = m_stop_udata.get();
    ret = epoll_ctl(m_fd, EPOLL_CTL_ADD, m_stop_event_fd, &event);
    
    if(ret < 0)
    {
        LOG_FATAL("stop event epoll_ctl failed in Poller_Executor::Poller_Executor");
    }
}

template<typename T>
bool Poller_Executor<T>::register_connection(std::shared_ptr<Connection<T>> connection)
{
    struct epoll_event event;
    event.data.ptr = connection.get();
    event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
    connection->m_poller_executor = this;
    connection->m_self = connection;
    
    if(!connection->m_init_cb(connection))
    {
        close(connection->m_fd);
        connection->m_closed.store(true, std::memory_order_relaxed);
        connection->m_self.reset();
        return false;
    }
    
    int32 ret = epoll_ctl(m_fd, EPOLL_CTL_ADD, connection->m_fd, &event);

    if(ret < 0)
    {
        LOG_FATAL("epoll_ctl failed in Poller_Executor::register_connection: %s", strerror(errno));
        close(connection->m_fd);
        connection->m_closed.store(true, std::memory_order_relaxed);
        connection->m_be_closed_cb(connection);
        connection->m_self.reset();
        
        return false;
    }

    return true;
}

template<typename T>
void Poller_Executor<T>::close_connection(std::shared_ptr<Connection<T>> connection)
{
    m_close_queue.push_direct(connection);
    uint64 data = 1;
    int32 num = write(m_close_event_fd, &data, sizeof(uint64));

    if(num != sizeof(uint64))
    {
        LOG_FATAL("write m_close_event_fd failed in Poller_Executor::close_connection");
    }
}

template<typename T>
void Poller_Executor<T>::stop()
{
    uint64 data = 1;
    int32 num = write(m_stop_event_fd, &data, sizeof(uint64));
    
    if(num != sizeof(uint64))
    {
        LOG_FATAL("write m_stop_event_fd failed in Poller_Executor::stop");
    }
}

template<typename T>
void Poller_Executor<T>::write_connection(std::shared_ptr<Connection<T>> connection)
{
    m_write_queue.push_direct(connection);
    uint64 data = 1;
    int32 num = write(m_write_event_fd, &data, sizeof(uint64));
    
    if(num != sizeof(uint64))
    {
        LOG_FATAL("write m_write_event_fd failed in Poller_Executor::write_connection");
    }
}

template<typename T>
void Poller_Executor<T>::do_write(std::shared_ptr<Connection<T>> connection)
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
            LOG_FATAL("write return 0, it's impossible, in Poller_Executor::do_write(arg)");
        }
        
        if(num <= 0)
        {
            int ret = epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, NULL);
            
            if(ret < 0)
            {
                LOG_FATAL("epoll_ctl EPOLL_CTL_DEL failed in Poller_Executor::do_write: %s", strerror(errno));
            }

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

template<typename T>
void Poller_Executor<T>::do_write()
{
    uint64 data = 0;
    int32 num = read(m_write_event_fd, &data, sizeof(uint64));

    if(num != sizeof(uint64))
    {
        LOG_FATAL("read m_write_event_fd failed in Poller_Executor::do_write");
        
        return;
    }
    
    std::list<std::shared_ptr<Connection<T>>> write_queue;

    if(m_write_queue.pop(write_queue))
    {
        for(auto &connection : write_queue)
        {
            if(!connection->m_closed.load(std::memory_order_relaxed))
            {
                do_write(connection);
            }
        }
    }
}

template<typename T>
void Poller_Executor<T>::do_close()
{
    uint64 data = 0;
    int32 num = read(m_close_event_fd, &data, sizeof(uint64));
    
    if(num != sizeof(uint64))
    {
        LOG_FATAL("read m_close_event_fd failed in Poller_Executor::do_close");
        
        return;
    }

    std::list<std::shared_ptr<Connection<T>>> close_queue;

    if(m_close_queue.pop(close_queue))
    {
        for(auto &connection : close_queue)
        {
            if(!connection->m_closed.load(std::memory_order_relaxed))
            {
                int32 fd = connection->m_fd;
                int ret = epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, NULL);

                if(ret < 0)
                {
                    LOG_FATAL("epoll_ctl EPOLL_CTL_DEL failed in Poller_Executor::do_close: %s", strerror(errno));
                }
                
                close(fd);
                connection->m_self.reset();
                connection->m_closed.store(true, std::memory_order_relaxed);
                connection->m_close_cb(connection);
            }
        }
    }
}

template<typename T>
void Poller_Executor<T>::run_in_loop()
{
    struct epoll_event events[2048];
    int32 fd_num = epoll_wait(m_fd, events, 2048, -1);
    
    if(fd_num < 0)
    {
        LOG_DEBUG_ERROR("epoll_wait failed in Poller_Executor::run_in_loop %s", strerror(errno));

        return;
    }

    for(auto i = 0; i < fd_num; ++i)
    {
        Connection<T> *connection = static_cast<Connection<T>*>(events[i].data.ptr);
        int32 fd = connection->m_fd;
        uint32 event = events[i].events;

        if(event & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
        {
            if(connection->m_closed.load(std::memory_order_relaxed))
            {
                continue;
            }

            int ret = epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, NULL);

            if(ret < 0)
            {
                LOG_FATAL("epoll_ctl EPOLL_CTL_DEL failed in Poller_Executor::run_in_loop: %s", strerror(errno));
            }

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
            else if(fd == m_stop_event_fd)
            {
                stop();
                close(m_fd);
                break;
            }
            else
            {
                if(connection->m_closed.load(std::memory_order_relaxed))
                {
                    continue;
                }
                
                while(true)
                {
                    const uint32 REQ_SIZE = 102400; // 100k
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
                        int ret = epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, NULL);

                        if(ret < 0)
                        {
                            LOG_FATAL("epoll_ctl EPOLL_CTL_DEL failed in Poller_Executor::run_in_loop (EPOLLIN): %s", strerror(errno));
                        }

                        close(fd);
                        connection->m_closed.store(true, std::memory_order_relaxed);
                        connection->m_be_closed_cb(connection->shared_from_this());
                        connection->m_self.reset();

                        break;
                    }
                    
                    message_chunk->write_ptr(num);
                    recv_queue.push(message_chunk.release());
                    connection->parse();

                    if(num < REQ_SIZE)
                    {
                        break;
                    }
                }
            }
        }

        if(event & EPOLLOUT)
        {
            if(connection->m_closed.load(std::memory_order_relaxed))
            {
                continue;
            }
            
            do_write(connection->shared_from_this());
        }
    }
}

template class Poller_Executor<Json>;
template class Poller_Executor<Wsock>;

}
}
