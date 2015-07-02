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
 *   @date: 2015-06-23 16:50:43                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "fly/net/client.hpp"
#include "fly/base/logger.hpp"

namespace fly {
namespace net {

Client::Client(const Addr &addr, std::function<void(std::shared_ptr<Connection>)> cb, std::shared_ptr<Poller> poller, std::shared_ptr<Parser> parser)
{
    m_addr = addr;
    m_cb = cb;
    m_poller = poller;
    m_parser = parser;
}

bool Client::connect()
{
    int32 fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(fd < 0)
    {
        LOG_FATAL("socket failed in Client::connect");

        return false;
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, m_addr.m_host.c_str(), &server_addr.sin_addr);
    server_addr.sin_port = htons(m_addr.m_port);

    if(::connect(fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        LOG_FATAL("connect failed in Client::connect, server addr is %s:%d", m_addr.m_host.c_str(), m_addr.m_port);

        return false;
    }

    std::shared_ptr<Connection> connection = std::make_shared<Connection>(fd, m_addr);
    m_cb(connection);
    m_parser->register_connection(connection);
    m_poller->register_connection(connection);
    m_id = connection->id();

    return true;
}

uint64 Client::id()
{
    return m_id;
}

}
}
