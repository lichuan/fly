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
using fly::net::Json;

class Test_Client : public fly::base::Singleton<Test_Client>
{
public:
    bool init(std::shared_ptr<fly::net::Connection<Json>> connection)
    {
        rapidjson::Document doc;
        doc.SetObject();
        rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
        doc.AddMember("msg_type", 9922, allocator); 
        doc.AddMember("msg_cmd", 2223333, allocator);
        connection->send(doc);
        return true;
    }
    
    void dispatch(std::unique_ptr<fly::net::Message<Json>> connection)
    {
        CONSOLE_LOG_INFO("disaptch message");
    }
    
    void close(std::shared_ptr<fly::net::Connection<Json>> connection)
    {
        CONSOLE_LOG_INFO("close connection from %s:%d", connection->peer_addr().m_host.c_str(), connection->peer_addr().m_port);
    }
    
    void be_closed(std::shared_ptr<fly::net::Connection<Json>> connection)
    {
        CONSOLE_LOG_INFO("connection from %s:%d be closed", connection->peer_addr().m_host.c_str(), connection->peer_addr().m_port);
    }
    
    void main()
    {
        //init library
        fly::init();
        
        //init logger
        fly::base::Logger::instance()->init(fly::base::DEBUG, "client", "./log/");
        std::shared_ptr<fly::net::Poller<Json>> poller(new fly::net::Poller<Json>(4));
        poller->start();
        
        int i = 1;
        while(i-- > 0)
        {
            
            std::unique_ptr<fly::net::Client<Json>> client(new fly::net::Client<Json>(fly::net::Addr("127.0.0.1", 8088),
                                                                          std::bind(&Test_Client::init, this, _1),
                                                                          std::bind(&Test_Client::dispatch, this, _1),
                                                                          std::bind(&Test_Client::close, this, _1),
                                                                          std::bind(&Test_Client::be_closed, this, _1),
                                                                          poller));
            if(client->connect(1000))
            {
                CONSOLE_LOG_INFO("connect to server ok");
            }
            else
            {
                CONSOLE_LOG_INFO("connect to server failed");
            }
        }
        
        poller->wait();
    }
};

int main()
{
    Test_Client::instance()->main();
}
