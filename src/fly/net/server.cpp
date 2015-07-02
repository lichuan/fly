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

Server::Server(const Addr &addr,
               std::function<bool(std::shared_ptr<Connection>)> allow_cb,
               std::function<void(std::shared_ptr<Connection>)> init_cb,
               std::function<void(std::unique_ptr<Message_Pack>)> dispatch_cb,
               std::function<void(std::shared_ptr<Connection>)> close_cb,
               std::function<void(std::shared_ptr<Connection>)> be_closed_cb,
               std::shared_ptr<Poller> poller, std::shared_ptr<Parser> parser)
{
    m_poller = poller;
    m_parser = parser;
    
    m_acceptor.reset(new Acceptor(addr, [=](std::shared_ptr<Connection> connection)
    {
        if(allow_cb(connection))
        {
            connection->m_init_cb = init_cb;
            connection->m_dispatch_cb = dispatch_cb;
            connection->m_close_cb = close_cb;
            connection->m_be_closed_cb = be_closed_cb;
            m_parser->register_connection(connection);
            m_poller->register_connection(connection);
        }
        else
        {
            close(connection->m_fd);
            LOG_INFO("connection from %s:%d is not allowed", connection->peer_addr().m_host.c_str(), connection->peer_addr().m_port);
        }
    }));
}

Server::Server(const Addr &addr,
               std::function<bool(std::shared_ptr<Connection>)> allow_cb,
               std::function<void(std::shared_ptr<Connection>)> init_cb,
               std::function<void(std::unique_ptr<Message_Pack>)> dispatch_cb,
               std::function<void(std::shared_ptr<Connection>)> close_cb,
               std::function<void(std::shared_ptr<Connection>)> be_closed_cb,
               uint32 poller_num, uint32 parser_num)
{
    m_poller.reset(new Poller(poller_num));
    m_parser.reset(new Parser(parser_num));

    m_acceptor.reset(new Acceptor(addr, [=](std::shared_ptr<Connection> connection)
    {
        if(allow_cb(connection))
        {
            connection->m_init_cb = init_cb;
            connection->m_dispatch_cb = dispatch_cb;
            connection->m_close_cb = close_cb;
            connection->m_be_closed_cb = be_closed_cb;
            m_parser->register_connection(connection);
            m_poller->register_connection(connection);
        }
        else
        {
            close(connection->m_fd);
            LOG_INFO("connection from %s:%d is not allowed", connection->peer_addr().m_host.c_str(), connection->peer_addr().m_port);
        }
    }));
}

void Server::start()
{
    if(m_poller.unique())
    {
        m_poller->start();
    }

    if(m_parser.unique())
    {
        m_parser->start();
    }

    m_acceptor->start();
}

void Server::wait()
{
    m_acceptor->wait();
    
    if(m_poller.unique())
    {
        m_poller->wait();
    }
    
    if(m_parser.unique())
    {
        m_parser->wait();
    }
}

}
}
