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

#include "fly/net/server.hpp"
#include <unistd.h>

namespace fly {
namespace net {

Server::Server(const Addr &addr, std::function<bool(std::shared_ptr<Connection>)> cb, std::shared_ptr<Poller> poller, std::shared_ptr<Parser> parser)
{
    m_poller = poller;
    m_parser = parser;
    
    m_acceptor.reset(new Acceptor(addr, [&, cb](std::shared_ptr<Connection> connection)
    {
        if(cb(connection))
        {
            m_parser->register_connection(connection);
            m_poller->register_connection(connection);
        }
        else
        {
            close(connection->m_fd);
        }
    }));
}

Server::Server(const Addr &addr, std::function<bool(std::shared_ptr<Connection>)> cb, uint32 poller_num, uint32 parser_num)
{
    m_poller.reset(new Poller(poller_num));
    m_parser.reset(new Parser(parser_num));

    m_acceptor.reset(new Acceptor(addr, [&, cb](std::shared_ptr<Connection> connection)
    {
        if(cb(connection))
        {
            m_parser->register_connection(connection);
            m_poller->register_connection(connection);
        }
        else
        {
            close(connection->m_fd);
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
