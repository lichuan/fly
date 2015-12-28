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
 *   @date: 2015-06-23 16:45:43                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__NET__MESSAGE
#define FLY__NET__MESSAGE

#include <memory>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "fly/base/common.hpp"

namespace fly {
namespace net {

class Connection;

class Message
{
    friend class Connection;
    
public:
    Message(std::shared_ptr<Connection> connection);
    rapidjson::Document& doc();
    const std::string& raw_data();
    uint32 type();
    uint32 cmd();
    uint32 length();
    std::shared_ptr<Connection> get_connection();
    
private:
    rapidjson::Document m_doc;
    std::shared_ptr<Connection> m_connection;
    std::string m_raw_data;
    uint32 m_length;
    uint32 m_type;
    uint32 m_cmd;
};

}
}

#endif
