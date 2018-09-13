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
 *   @date: 2015-06-23 16:48:13                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__NET__SERVER
#define FLY__NET__SERVER

#include "fly/net/acceptor.hpp"
#include "fly/net/poller.hpp"
#include "fly/net/message.hpp"

namespace fly {
namespace net {

template<typename T>
class Server
{
public:
    Server(const Addr &addr,
           std::function<bool(std::shared_ptr<Connection<T>>)> init_cb,
           std::function<void(std::unique_ptr<Message<T>>)> dispatch_cb,
           std::function<void(std::shared_ptr<Connection<T>>)> close_cb,
           std::function<void(std::shared_ptr<Connection<T>>)> be_closed_cb,
           std::shared_ptr<Poller<T>> poller, uint32 max_msg_length = 1024 * 1024 * 1024);
    Server(const Addr &addr,
           std::function<bool(std::shared_ptr<Connection<T>>)> init_cb,
           std::function<void(std::unique_ptr<Message<T>>)> dispatch_cb,
           std::function<void(std::shared_ptr<Connection<T>>)> close_cb,
           std::function<void(std::shared_ptr<Connection<T>>)> be_closed_cb,
           uint32 poller_num = 1, uint32 max_msg_length = 1024 * 1024 * 1024);
    void wait();
    bool start();
    void stop();
    
private:
    std::unique_ptr<Acceptor<T>> m_acceptor;
    std::shared_ptr<Poller<T>> m_poller;
};

}
}

#endif
