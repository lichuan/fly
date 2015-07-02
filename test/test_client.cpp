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
 *   @date: 2015-06-10 13:34:21                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <unistd.h>
#include "fly/init.hpp"
#include "fly/base/logger.hpp"
#include "fly/net/client.hpp"
#include "fly/net/message_pack.hpp"

using namespace std;
using namespace fly::net;

int main()
{
    //init library
    fly::init();

    //init logger
    fly::base::Logger::instance()->init(fly::base::DEBUG, "client", "./log/");
    
    //test logger
    LOG_INFO("this is a msg to logger, I am %s, 1024 * 1024 = %d", "lichuan", 1024 * 1024);

    std::shared_ptr<Poller> poller(new Poller(1));
    std::shared_ptr<Parser> parser(new Parser(1));
    std::shared_ptr<Connection> tcp_connection;
    std::unique_ptr<Client> client(new Client(Addr("127.0.0.1", 8899),
    [&](std::shared_ptr<Connection> connection)
    {
        tcp_connection = connection;
        LOG_INFO("client connection %s:%d", connection->peer_addr().m_host.c_str(), connection->peer_addr().m_port);
    }, [](std::shared_ptr<Message_Pack>){}, [](std::shared_ptr<Connection>){}, [](std::shared_ptr<Connection> connection)
    {
        LOG_INFO("connection from %s:%d be closed", connection->peer_addr().m_host.c_str(), connection->peer_addr().m_port);
    }, poller, parser));

    if(client->connect())
    {
        LOG_INFO("connect to server ok");
    }

    poller->start();
    parser->start();
    poller->wait();
    parser->wait();
}
