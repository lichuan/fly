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
 *   @date: 2015-06-23 16:49:58                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__NET__CLIENT
#define FLY__NET__CLIENT

#include "fly/net/poller.hpp"
#include "fly/net/message.hpp"

namespace fly {
namespace net {

template<typename T>
class Client
{
public:
    Client(const Addr &addr,
           std::function<bool(std::shared_ptr<Connection<T>>)> init_cb,
           std::function<void(std::unique_ptr<Message<T>>)> dispatch_cb,
           std::function<void(std::shared_ptr<Connection<T>>)> close_cb,
           std::function<void(std::shared_ptr<Connection<T>>)> be_closed_cb,
           std::shared_ptr<Poller<T>> poller, uint32 max_msg_length = 1024 * 1024 * 1024);
    Client(const Addr &addr);
    bool connect(int32 timeout = -1);
    uint64 id();
    
private:
    bool m_only_check;
    uint32 m_max_msg_length;
    uint64 m_id;
    Addr m_addr;
    std::shared_ptr<Poller<T>> m_poller;
    std::function<void(std::shared_ptr<Connection<T>>)> m_close_cb;
    std::function<void(std::shared_ptr<Connection<T>>)> m_be_closed_cb;
    std::function<bool(std::shared_ptr<Connection<T>>)> m_init_cb;
    std::function<void(std::unique_ptr<Message<T>>)> m_dispatch_cb;
};

}
}

#endif
