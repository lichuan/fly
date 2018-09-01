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

//websocket
class Wsock {};

//json
class Json {};

//google protobuf
class Proto {};

template<typename T>
class Connection;

template<typename T>
class Message {};

template<>
class Message<Json>
{
    friend class Connection<Json>;
    
public:
    Message(std::shared_ptr<Connection<Json>> connection);
    rapidjson::Document& doc();
    std::shared_ptr<rapidjson::Document> doc_shared();
    const std::string& raw_data();
    uint32 type();
    uint32 cmd();
    uint32 length();
    std::shared_ptr<Connection<Json>> get_connection();
    
private:
    std::shared_ptr<rapidjson::Document> m_doc;
    std::shared_ptr<Connection<Json>> m_connection;
    std::string m_raw_data;
    uint32 m_length;
    uint32 m_type;
    uint32 m_cmd;
};

template<>
class Message<Proto>
{
    friend class Connection<Proto>;
    
public:
    Message(std::shared_ptr<Connection<Proto>> connection);
    rapidjson::Document& doc();
    std::shared_ptr<rapidjson::Document> doc_shared();
    const std::string& raw_data();
    uint32 type();
    uint32 cmd();
    uint32 length();
    std::shared_ptr<Connection<Proto>> get_connection();
    
private:
    std::shared_ptr<rapidjson::Document> m_doc;
    std::shared_ptr<Connection<Proto>> m_connection;
    std::string m_raw_data;
    uint32 m_length;
    uint32 m_type;
    uint32 m_cmd;
};

template<>
class Message<Wsock>
{
    friend class Connection<Wsock>;
    
public:
    Message(std::shared_ptr<Connection<Wsock>> connection);
    rapidjson::Document& doc();
    std::shared_ptr<rapidjson::Document> doc_shared();
    const std::string& raw_data();
    uint32 type();
    uint32 cmd();
    uint32 length();
    std::shared_ptr<Connection<Wsock>> get_connection();
    
private:
    std::shared_ptr<rapidjson::Document> m_doc;
    std::shared_ptr<Connection<Wsock>> m_connection;
    std::string m_raw_data;
    uint32 m_length;
    uint32 m_type;
    uint32 m_cmd;
};

}
}

#endif
