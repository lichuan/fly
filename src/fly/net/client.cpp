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
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <netinet/in.h>
#include "fly/net/client.hpp"
#include "fly/base/logger.hpp"

namespace fly {
namespace net {

template<typename T>
Client<T>::Client(const Addr &addr,
                  std::function<bool(std::shared_ptr<Connection<T>>)> init_cb,
                  std::function<void(std::unique_ptr<Message<T>>)> dispatch_cb,
                  std::function<void(std::shared_ptr<Connection<T>>)> close_cb,
                  std::function<void(std::shared_ptr<Connection<T>>)> be_closed_cb,
                  std::shared_ptr<Poller<T>> poller, uint32 max_msg_length)
{
    m_addr = addr;
    m_init_cb = init_cb;
    m_dispatch_cb = dispatch_cb;
    m_close_cb = close_cb;
    m_be_closed_cb = be_closed_cb;
    m_poller = poller;
    m_only_check = false;
    m_max_msg_length = max_msg_length;
}

template<typename T>
Client<T>::Client(const Addr &addr)
{
    m_addr = addr;
    m_only_check = true;
}

template<typename T>
bool Client<T>::connect(int32 timeout)
{
    struct addrinfo hint;
    hint.ai_flags = 0;
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = 0;
    struct addrinfo *result, *iter;
    int32 ret = getaddrinfo(m_addr.m_host.c_str(), base::to_string(m_addr.m_port).c_str(), &hint, &result);

    if(ret != 0)
    {
        LOG_DEBUG_FATAL("resolve dns: %s:%u failed in client::connect: %s", m_addr.m_host.c_str(), m_addr.m_port, gai_strerror(ret));
        
        return false;
    }
    
    for(iter = result; iter != NULL; iter = iter->ai_next)
    {
        int32 fd = socket(AF_INET, SOCK_STREAM, 0);
    
        if(fd < 0)
        {
            LOG_FATAL("socket failed in Client::connect: %s", strerror(errno));

            return false;
        }

        int32 flags = fcntl(fd, F_GETFL, 0);

        if(flags == -1)
        {
            LOG_FATAL("fcntl F_GETFL failed in Client::connect");
            close(fd);
        
            return false;
        }

        flags |= O_NONBLOCK;

        if(fcntl(fd, F_SETFL, flags) == -1)
        {
            LOG_FATAL("fnctl F_SETFL O_NONBLOCK failed in Client::connect");
            close(fd);
        
            return false;
        }
        
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &((sockaddr_in*)(iter->ai_addr))->sin_addr, ip, INET_ADDRSTRLEN);
        LOG_DEBUG_INFO("resolve ip success in client::connect host: %s, ip: %s", m_addr.m_host.c_str(), ip);
        
        if(::connect(fd, iter->ai_addr, sizeof(sockaddr)) < 0)
        {
            if(errno != EINPROGRESS)
            {        
                LOG_DEBUG_FATAL("connect failed in Client::connect, host: %s, ip: %s, port: %d %s", m_addr.m_host.c_str(), ip, m_addr.m_port, strerror(errno));
                close(fd);
                
                continue;
            }

            struct pollfd fds;
            fds.fd = fd;
            fds.events = POLLOUT;
            int32 ret = poll(&fds, 1, timeout);

            if(ret < 0)
            {
                LOG_FATAL("connect failed poll return < 0");
                close(fd);
                
                continue;
            }

            if(ret == 0)
            {
                LOG_DEBUG_ERROR("connect failed, poll timeout");
                close(fd);
                
                continue;
            }
        
            socklen_t len = sizeof(ret);

            if(getsockopt(fd, SOL_SOCKET, SO_ERROR, &ret, &len) < 0)
            {
                LOG_FATAL("connect failed, getsockopt return < 0");
                close(fd);
            
                continue;
            }

            if(ret != 0)
            {
                LOG_DEBUG_ERROR("connect failed, getsockopt error: %s", strerror(ret));
                close(fd);
            
                continue;
            }
        }

        if(!m_only_check)
        {
            std::shared_ptr<Connection<T>> connection = std::make_shared<Connection<T>>(fd, m_addr);
            m_id = connection->m_id_allocator.new_id();
            connection->set_passive(false);
            connection->m_max_msg_length = m_max_msg_length;
            connection->m_id = m_id;
            connection->m_init_cb = m_init_cb;
            connection->m_dispatch_cb = m_dispatch_cb;
            connection->m_close_cb = m_close_cb;
            connection->m_be_closed_cb = m_be_closed_cb;
            
            if(!m_poller->register_connection(connection))
            {
                LOG_DEBUG_INFO("register_connection from %s:%d failed", connection->peer_addr().m_host.c_str(), connection->peer_addr().m_port);
                return false;
            }
        }
        
        return true;
    }
    
    return false;
}

template<typename T>
uint64 Client<T>::id()
{
    return m_id;
}

template class Client<Json>;
//template class Client<Wsock>;
template class Client<Proto>;

}
}
