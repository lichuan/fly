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
#include "fly/net/parser.hpp"

namespace fly {
namespace net {

class Server
{
public:
    Server(const Addr &addr, std::function<bool(std::shared_ptr<Connection>)> cb, std::shared_ptr<Poller> poller, std::shared_ptr<Parser> parser);
    Server(const Addr &addr, std::function<bool(std::shared_ptr<Connection>)> cb, uint32 poller_num = 1, uint32 parser_num = 1);
    void wait();
    void start();
    
private:
    std::unique_ptr<Acceptor> m_acceptor;
    std::shared_ptr<Poller> m_poller;
    std::shared_ptr<Parser> m_parser;
};

}
}

#endif
