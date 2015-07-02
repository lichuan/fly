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
#include "fly/net/server.hpp"
#include "fly/net/holder.hpp"
#include "fly/net/message_pack.hpp"

using namespace std;
using namespace fly::net;

class Unique_Holder : public Holder
{
public:
    virtual void close_connection(std::shared_ptr<Connection> connection)
    {
        LOG_INFO("close this connection");
    }
    
    virtual void connection_be_closed(std::shared_ptr<Connection> connection)
    {
        LOG_INFO("this connection be closed");
    }
    
    virtual void dispatch_message(std::unique_ptr<Message_Pack> pack)
    {
        LOG_INFO("dispatch a message on this connection");
    }
    
    virtual void init_connection(std::shared_ptr<Connection> connection)
    {
        LOG_INFO("init this connection");
    }
};

int main()
{
    //init library
    fly::init();

    //init logger
    fly::base::Logger::instance()->init(fly::base::DEBUG, "server", "./log/");
    
    //test logger
    LOG_INFO("this is a msg to logger, I am %s, 1024 * 1024 = %d", "lichuan", 1024 * 1024);

    //test tcp network
    Unique_Holder unique_holder;
    std::vector<std::shared_ptr<Connection>> conn_vec;
    
    std::unique_ptr<Server> server(new Server(Addr("127.0.0.1", 8899), [&](std::shared_ptr<Connection> connection)
    {
        LOG_INFO("a new connection come from %s:%d", connection->peer_addr().m_host.c_str(), connection->peer_addr().m_port);
        connection->holder(&unique_holder);
        conn_vec.push_back(connection);
        
        return true; //allow this connection
    }));
    
    server->start();
    LOG_INFO("start server ok!");
    server->wait();
}
