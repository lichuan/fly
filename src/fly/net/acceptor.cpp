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
 *   @date: 2015-06-22 19:59:59                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <errno.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "fly/base/logger.hpp"
#include "fly/net/acceptor.hpp"

namespace fly {
namespace net {

Acceptor::Acceptor(const Addr &addr, std::function<void(std::shared_ptr<Connection>)> cb)
{
    m_listen_addr = addr;
    m_cb = cb;
}

bool Acceptor::start()
{
    int32 listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(listen_fd < 0)
    {
        LOG_FATAL("socket failed in Acceptor::start, listen addr is %s:%d", m_listen_addr.m_host.c_str(), m_listen_addr.m_port);
        
        return false;
    }
    
    int32 opt = 1;
    int32 opt_len = sizeof(opt);

    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, opt_len) < 0)
    {
        LOG_FATAL("setsockopt failed in Acceptor::start, listen addr is %s:%d", m_listen_addr.m_host.c_str(), m_listen_addr.m_port);
        
        return false;
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, m_listen_addr.m_host.c_str(), &server_addr.sin_addr);
    server_addr.sin_port = htons(m_listen_addr.m_port);

    if(bind(listen_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        LOG_FATAL("bind failed in Acceptor::start, listen addr is %s:%d", m_listen_addr.m_host.c_str(), m_listen_addr.m_port);
        
        return false;
    }
    
    if(listen(listen_fd, SOMAXCONN) < 0)
    {
        LOG_FATAL("listen failed in Acceptor::start, listen addr is %s:%d", m_listen_addr.m_host.c_str(), m_listen_addr.m_port);
        
        return false;
    }
    
    std::thread tmp([=]()
    {
        while(true)
        {
            struct sockaddr_in client_addr;
            socklen_t length = sizeof(sockaddr_in);
            int32 client_fd = accept4(listen_fd, (sockaddr*)&client_addr, &length, SOCK_NONBLOCK);
            
            if(client_fd < 0)
            {
                LOG_FATAL("accept4 failed in Acceptor::start");
                
                continue;
            }
            
            char host[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, host, INET_ADDRSTRLEN);
            uint16 port = ntohs(client_addr.sin_port);
            LOG_INFO("new connection from %s:%d arrived", host, port);
            m_cb(std::make_shared<Connection>(client_fd, Addr(host, port)));
        }
    });
    
    m_thread = std::move(tmp);

    return true;
}

void Acceptor::wait()
{
    m_thread.join();
}

}
}
