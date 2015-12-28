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
 *   @email: lichuan@lichuan.me                                        *
 *   @github: https://github.com/lichuan/fly                           *
 *   @date: 2015-06-23 16:48:13                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__NET__SERVER
#define FLY__NET__SERVER

#include "fly/net/acceptor.hpp"
#include "fly/net/poller.hpp"
#include "fly/net/parser.hpp"
#include "fly/net/message.hpp"

namespace fly {
namespace net {

class Server
{
public:
    Server(const Addr &addr,
           std::function<bool(std::shared_ptr<Connection>)> allow_cb,
           std::function<void(std::shared_ptr<Connection>)> init_cb,
           std::function<void(std::unique_ptr<Message>)> dispatch_cb,
           std::function<void(std::shared_ptr<Connection>)> close_cb,
           std::function<void(std::shared_ptr<Connection>)> be_closed_cb,
           std::shared_ptr<Poller> poller, std::shared_ptr<Parser> parser);
    Server(const Addr &addr,
           std::function<bool(std::shared_ptr<Connection>)> allow_cb,
           std::function<void(std::shared_ptr<Connection>)> init_cb,
           std::function<void(std::unique_ptr<Message>)> dispatch_cb,
           std::function<void(std::shared_ptr<Connection>)> close_cb,
           std::function<void(std::shared_ptr<Connection>)> be_closed_cb,
           uint32 poller_num = 1, uint32 parser_num = 1);
    void wait();
    bool start();
    
private:
    std::unique_ptr<Acceptor> m_acceptor;
    std::shared_ptr<Poller> m_poller;
    std::shared_ptr<Parser> m_parser;
};

}
}

#endif
