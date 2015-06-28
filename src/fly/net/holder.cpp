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
 *   @date: 2015-06-27 13:15:59                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "fly/net/holder.hpp"

namespace fly {
namespace net {

void Holder::connection_be_closed(std::shared_ptr<Connection> connection)
{
    {
        std::lock_guard<std::mutex> guard(m_connection_mutex);
        uint64 id = connection->id();
        
        if(m_alive_ids.find(id) == m_alive_ids.end())
        {
            return;
        }
        
        m_alive_ids.erase(id);
    }
    
    connection_be_closed(connection);
}

void Holder::init_connection(std::shared_ptr<Connection> connection)
{
    {
        std::lock_guard<std::mutex> guard(m_connection_mutex);
        m_alive_ids.insert(connection->id());
    }

    init_connection(connection);
}

void Holder::close_connection(std::shared_ptr<Connection> connection)
{
    {
        std::lock_guard<std::mutex> guard(m_connection_mutex);
        uint64 id = connection->id();
        
        if(m_alive_ids.find(id) == m_alive_ids.end())
        {
            return;
        }
    
        m_alive_ids.erase(id);
    }
    
    close_connection(connection);
}

}
}
