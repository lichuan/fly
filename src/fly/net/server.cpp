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
 *   @date: 2015-06-23 16:49:13                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <unistd.h>
#include "fly/net/server.hpp"
#include "fly/base/logger.hpp"

namespace fly {
namespace net {

template<typename T>
class Connection;

template<typename T>
Server<T>::Server(const Addr &addr,
               std::function<bool(std::shared_ptr<Connection<T>>)> allow_cb,
               std::function<void(std::shared_ptr<Connection<T>>)> init_cb,
               std::function<void(std::unique_ptr<Message<T>>)> dispatch_cb,
               std::function<void(std::shared_ptr<Connection<T>>)> close_cb,
               std::function<void(std::shared_ptr<Connection<T>>)> be_closed_cb,
               std::shared_ptr<Poller<T>> poller)
{
    m_poller = poller;

    m_acceptor.reset(new Acceptor<T>(addr, [=](std::shared_ptr<Connection<T>> connection)
    {
        if(allow_cb(connection))
        {
            connection->m_id = connection->m_id_allocator.new_id();
            connection->m_init_cb = init_cb;
            connection->m_dispatch_cb = dispatch_cb;
            connection->m_close_cb = close_cb;
            connection->m_be_closed_cb = be_closed_cb;
            m_poller->register_connection(connection);
        }
        else
        {
            close(connection->m_fd);
            LOG_INFO("connection from %s:%d is not allowed", connection->peer_addr().m_host.c_str(), connection->peer_addr().m_port);
        }
    }));
}

template<typename T>
Server<T>::Server(const Addr &addr,
               std::function<bool(std::shared_ptr<Connection<T>>)> allow_cb,
               std::function<void(std::shared_ptr<Connection<T>>)> init_cb,
               std::function<void(std::unique_ptr<Message<T>>)> dispatch_cb,
               std::function<void(std::shared_ptr<Connection<T>>)> close_cb,
               std::function<void(std::shared_ptr<Connection<T>>)> be_closed_cb,
               uint32 poller_num)
{
    m_poller.reset(new Poller<T>(poller_num));
    
    m_acceptor.reset(new Acceptor<T>(addr, [=](std::shared_ptr<Connection<T>> connection)
    {
        if(allow_cb(connection))
        {
            connection->m_id = connection->m_id_allocator.new_id();
            connection->m_init_cb = init_cb;
            connection->m_dispatch_cb = dispatch_cb;
            connection->m_close_cb = close_cb;
            connection->m_be_closed_cb = be_closed_cb;
            m_poller->register_connection(connection);
        }
        else
        {
            close(connection->m_fd);
            LOG_INFO("connection from %s:%d is not allowed", connection->peer_addr().m_host.c_str(), connection->peer_addr().m_port);
        }
    }));
}

template<typename T>
bool Server<T>::start()
{
    if(m_poller.unique())
    {
        m_poller->start();
    }

    return m_acceptor->start();
}

template<typename T>
void Server<T>::stop()
{
    m_acceptor->stop();

    if(m_poller.unique())
    {
        m_poller->stop();
    }
}

template<typename T>
void Server<T>::wait()
{
    m_acceptor->wait();
    
    if(m_poller.unique())
    {
        m_poller->wait();
    }
}

template class Server<Json>;
template class Server<Wsock>;
template class Server<Proto>;

}
}
