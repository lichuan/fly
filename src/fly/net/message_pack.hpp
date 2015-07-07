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
 *   @date: 2015-06-23 16:45:43                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__NET__MESSAGE_PACK
#define FLY__NET__MESSAGE_PACK

#include <memory>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "fly/base/common.hpp"

namespace fly {
namespace net {

class Connection;

class Message_Pack
{
    friend class Connection;
    
public:
    Message_Pack(std::shared_ptr<Connection> connection);
    rapidjson::Document& doc();
    const std::string& raw_data();
    uint32 message_type();
    uint32 message_cmd();
    
private:
    rapidjson::Document m_doc;
    std::shared_ptr<Connection> m_connection;
    std::string m_raw_data;
    uint32 m_message_type;
    uint32 m_message_cmd;
};

}
}

#endif
