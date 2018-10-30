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
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "fly/base/logger.hpp"
#include "fly/net/acceptor.hpp"

namespace fly {
namespace net {

template<typename T>
Acceptor<T>::Acceptor(const Addr &addr, std::function<void(std::shared_ptr<Connection<T>>)> cb)
{
    m_listen_addr = addr;
    m_cb = cb;
}

template<typename T>
bool Acceptor<T>::start()
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

    int32 flags = fcntl(listen_fd, F_GETFL, 0);

    if(flags == -1)
    {
        LOG_FATAL("set listen fd to nonblock failed 1 in Acceptor::start, listen addr is %s:%d", m_listen_addr.m_host.c_str(), m_listen_addr.m_port);

        return false;
    }

    flags |= O_NONBLOCK;

    if(fcntl(listen_fd, F_SETFL, flags) == -1)
    {
        LOG_FATAL("set listen fd to nonblock failed 2 in Acceptor::start, listen addr is %s:%d", m_listen_addr.m_host.c_str(), m_listen_addr.m_port);
        
        return false;
    }

    std::thread tmp([=]()
    {
        struct pollfd fds;
        fds.fd = listen_fd;
        fds.events = POLLIN;

        while(true)
        {
            int32 ret = poll(&fds, 1, 1000);

            if(ret < 0)
            {
                LOG_FATAL("poll listen fd return -1");

                continue;
            }

            if(!m_running.load(std::memory_order_relaxed))
            {
                break;
            }
            
            // timeout
            if(ret == 0)
            {
                continue;
            }

            if(!(fds.revents & POLLIN))
            {
                LOG_FATAL("poll listen fd return revents not contains POLLIN");

                continue;
            }

            struct sockaddr_in client_addr;
            socklen_t length = sizeof(sockaddr_in);
            int32 client_fd = accept4(listen_fd, (sockaddr*)&client_addr, &length, SOCK_NONBLOCK);

            if(client_fd < 0)
            {
                LOG_DEBUG_FATAL("accept4 return < 0: %d %s", errno, strerror(errno));
                
                if(errno == EMFILE)
                {
                    sleep(1);
                }

                continue;
            }
            
            char host[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, host, INET_ADDRSTRLEN);
            uint16 port = ntohs(client_addr.sin_port);
            LOG_DEBUG_INFO("new connection from %s:%d arrived", host, port);
            m_cb(std::make_shared<Connection<T>>(client_fd, Addr(host, port)));
        }
    });

    m_thread = std::move(tmp);
    
    return true;
}

template<typename T>
void Acceptor<T>::stop()
{
    m_running.store(false, std::memory_order_relaxed);
}

template<typename T>
void Acceptor<T>::wait()
{
    m_thread.join();
}

template class Acceptor<Json>;
template class Acceptor<Wsock>;
template class Acceptor<Proto>;

}
}
