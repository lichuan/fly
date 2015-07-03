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
#include "fly/net/client.hpp"
#include "fly/base/logger.hpp"

using namespace std::placeholders;

class Test_Client : public fly::base::Singleton<Test_Client>
{
public:
    void init(std::shared_ptr<fly::net::Connection> connection)
    {
        m_server_connection = connection;
    }
    
    void dispatch(std::unique_ptr<fly::net::Message_Pack> connection)
    {
        LOG_INFO("disaptch message");
    }
    
    void close(std::shared_ptr<fly::net::Connection> connection)
    {
        LOG_INFO("close connection from %s:%d", connection->peer_addr().m_host.c_str(), connection->peer_addr().m_port);
    }
    
    void be_closed(std::shared_ptr<fly::net::Connection> connection)
    {
        LOG_INFO("connection from %s:%d be closed", connection->peer_addr().m_host.c_str(), connection->peer_addr().m_port);
    }
    
    void main()
    {
        //init library
        fly::init();
        
        //init logger
        fly::base::Logger::instance()->init(fly::base::DEBUG, "client", "./log/");
        
        std::shared_ptr<fly::net::Poller> poller(new fly::net::Poller(1));
        std::shared_ptr<fly::net::Parser> parser(new fly::net::Parser(1));
        std::unique_ptr<fly::net::Client> client(new fly::net::Client(fly::net::Addr("127.0.0.1", 8899),
                                                                      std::bind(&Test_Client::init, this, _1),
                                                                      std::bind(&Test_Client::dispatch, this, _1),
                                                                      std::bind(&Test_Client::close, this, _1),
                                                                      std::bind(&Test_Client::be_closed, this, _1),
                                                                      poller, parser));

        if(client->connect())
        {
            LOG_INFO("connect to server ok");
        }

        poller->start();
        parser->start();
        poller->wait();
        parser->wait();
    }
    
private:
    std::shared_ptr<fly::net::Connection> m_server_connection;
};

int main()
{
    Test_Client::instance()->main();
}
