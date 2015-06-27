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
 *   @date: 2015-06-23 16:43:24                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__NET__HOLDER
#define FLY__NET__HOLDER

#include <unordered_set>
#include "fly/net/connection.hpp"
#include "fly/net/message_pack.hpp"

namespace fly {
namespace net {

class Holder
{
public:
    virtual ~Holder() = default;
    virtual void close_connection(std::shared_ptr<Connection> connection); //active
    virtual void connection_be_closed(std::shared_ptr<Connection> connection); //passive
    virtual void dispatch_message(Message_Pack *pack);
    
private:
    std::mutex m_mutex;
    std::unordered_set<uint64> m_alive_ids;
};

}
}

#endif
