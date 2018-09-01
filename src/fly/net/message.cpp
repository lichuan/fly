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
 *   @date: 2015-06-23 16:46:53                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "fly/net/message.hpp"

namespace fly {
namespace net {

//Json
Message<Json>::Message(std::shared_ptr<Connection<Json>> connection)
{
    m_connection = connection;
    m_doc = std::make_shared<rapidjson::Document>();
}

rapidjson::Document& Message<Json>::doc()
{
    return *m_doc;
}

std::shared_ptr<rapidjson::Document> Message<Json>::doc_shared()
{
    return m_doc;
}

uint32 Message<Json>::type()
{
    return m_type;
}

uint32 Message<Json>::cmd()
{
    return m_cmd;
}

uint32 Message<Json>::length()
{
    return m_length;
}

const std::string& Message<Json>::raw_data()
{
    return m_raw_data;
}

std::shared_ptr<Connection<Json>> Message<Json>::get_connection()
{
    return m_connection;
}

//Proto
Message<Proto>::Message(std::shared_ptr<Connection<Proto>> connection)
{
    m_connection = connection;
    m_doc = std::make_shared<rapidjson::Document>();
}

rapidjson::Document& Message<Proto>::doc()
{
    return *m_doc;
}

std::shared_ptr<rapidjson::Document> Message<Proto>::doc_shared()
{
    return m_doc;
}

uint32 Message<Proto>::type()
{
    return m_type;
}

uint32 Message<Proto>::cmd()
{
    return m_cmd;
}

uint32 Message<Proto>::length()
{
    return m_length;
}

const std::string& Message<Proto>::raw_data()
{
    return m_raw_data;
}

std::shared_ptr<Connection<Proto>> Message<Proto>::get_connection()
{
    return m_connection;
}

//Wsock
Message<Wsock>::Message(std::shared_ptr<Connection<Wsock>> connection)
{
    m_connection = connection;
    m_doc = std::make_shared<rapidjson::Document>();
}

rapidjson::Document& Message<Wsock>::doc()
{
    return *m_doc;
}

std::shared_ptr<rapidjson::Document> Message<Wsock>::doc_shared()
{
    return m_doc;
}

uint32 Message<Wsock>::type()
{
    return m_type;
}

uint32 Message<Wsock>::cmd()
{
    return m_cmd;
}

uint32 Message<Wsock>::length()
{
    return m_length;
}

const std::string& Message<Wsock>::raw_data()
{
    return m_raw_data;
}

std::shared_ptr<Connection<Wsock>> Message<Wsock>::get_connection()
{
    return m_connection;
}

}
}
