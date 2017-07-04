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
 *   @date: 2015-06-22 19:53:53                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__NET__ACCEPTOR
#define FLY__NET__ACCEPTOR

#include <memory>
#include <thread>
#include "fly/net/connection.hpp"

namespace fly {
namespace net {

template<typename T>
class Acceptor
{
public:
    Acceptor(const Addr &addr, std::function<void(std::shared_ptr<Connection<T>>)> cb);
    bool start();
    void stop();
    void wait();
    
private:
    std::function<void(std::shared_ptr<Connection<T>>)> m_cb;
    std::atomic<bool> m_running {true};
    Addr m_listen_addr;
    std::thread m_thread;
};

}
}

#endif
