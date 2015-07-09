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
 *   @date: 2015-06-22 18:22:00                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include "fly/net/connection.hpp"
#include "fly/net/poller_task.hpp"
#include "fly/base/logger.hpp"

namespace fly {
namespace net {

fly::base::ID_Allocator Connection::m_id_allocator;

Connection::~Connection()
{
    while(auto *message_chunk = m_recv_msg_queue.pop())
    {
        delete message_chunk;
    }

    while(auto *message_chunk = m_send_msg_queue.pop())
    {
        delete message_chunk;
    }
}

Connection::Connection(int32 fd, const Addr &peer_addr)
{
    m_fd = fd;
    m_peer_addr = peer_addr;
}

uint64 Connection::id()
{
    return m_id;
}

void Connection::send(rapidjson::Document &doc)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    send(buffer.GetString(), buffer.GetSize());
}

void Connection::send(const void *data, uint32 size)
{
    Message_Chunk *message_chunk = new Message_Chunk(size + sizeof(uint32));
    uint32 *uint32_ptr = (uint32*)message_chunk->read_ptr();
    *uint32_ptr = htonl(size);
    memcpy(message_chunk->read_ptr() + sizeof(uint32), data, size);
    message_chunk->write_ptr(size + sizeof(uint32));
    m_send_msg_queue.push(message_chunk);
    m_poller_task->write_connection(shared_from_this());
}

void Connection::close()
{
    m_poller_task->close_connection(shared_from_this());
}

const Addr& Connection::peer_addr()
{
    return m_peer_addr;
}

void Connection::parse()
{
    while(true)
    {
        char *msg_length_buf = (char*)(&m_cur_msg_length);
        uint32 remain_bytes = sizeof(uint32);
        
        if(m_cur_msg_length != 0)
        {
            goto after_parse_length;
        }
        
        if(m_recv_msg_queue.length() < sizeof(uint32))
        {
            break;
        }
        
        while(auto *message_chunk = m_recv_msg_queue.pop())
        {
            uint32 length = message_chunk->length();
            
            if(length < remain_bytes)
            {
                memcpy(msg_length_buf + sizeof(uint32) - remain_bytes, message_chunk->read_ptr(), length);
                remain_bytes -= length;
                delete message_chunk;
            }
            else
            {
                memcpy(msg_length_buf + sizeof(uint32) - remain_bytes, message_chunk->read_ptr(), remain_bytes);

                if(length == remain_bytes)
                {
                    delete message_chunk;
                }
                else
                {
                    message_chunk->read_ptr(remain_bytes);
                    m_recv_msg_queue.push_front(message_chunk);
                }
                
                break;
            }
        }
        
        m_cur_msg_length = ntohl(m_cur_msg_length);
        
    after_parse_length:
        if(m_recv_msg_queue.length() < m_cur_msg_length)
        {
            break;
        }
        
        const uint32 MAX_MSG_LEN = 102400;
        char msg_buf[MAX_MSG_LEN] = {0};
        char *data = msg_buf;
        bool is_new_buf = false;
        remain_bytes = m_cur_msg_length;
        
        if(m_cur_msg_length > MAX_MSG_LEN)
        {
            LOG_ERROR("message length exceed MAX_MSG_LEN(%d)", MAX_MSG_LEN);
            data = new char[m_cur_msg_length];
            is_new_buf = true;
        }
        else if(m_cur_msg_length > MAX_MSG_LEN / 2)
        {
            LOG_ERROR("message length exceed half of MAX_MSG_LEN(%d)", MAX_MSG_LEN);
        }
        
        while(auto *message_chunk = m_recv_msg_queue.pop())
        {
            uint32 length = message_chunk->length();

            if(length < remain_bytes)
            {
                memcpy(data + m_cur_msg_length - remain_bytes, message_chunk->read_ptr(), length);
                remain_bytes -= length;
                delete message_chunk;
            }
            else
            {
                memcpy(data + m_cur_msg_length - remain_bytes, message_chunk->read_ptr(), remain_bytes);

                if(length == remain_bytes)
                {
                    delete message_chunk;
                }
                else
                {
                    message_chunk->read_ptr(remain_bytes);
                    m_recv_msg_queue.push_front(message_chunk);
                }

                std::unique_ptr<Message> message(new Message(shared_from_this()));
                message->m_raw_data.assign(data, m_cur_msg_length);
                m_cur_msg_length = 0;

                if(is_new_buf)
                {
                    delete[] data;
                }
                
                rapidjson::Document &doc = message->doc();
                doc.Parse(message->m_raw_data.c_str());
                
                if(!doc.HasParseError())
                {
                    if(!doc.HasMember("msg_type"))
                    {
                        break;
                    }
                    
                    const rapidjson::Value &msg_type = doc["msg_type"];

                    if(!msg_type.IsUint())
                    {
                        break;
                    }

                    message->m_type = msg_type.GetUint();

                    if(!doc.HasMember("msg_cmd"))
                    {
                        break;
                    }

                    const rapidjson::Value &msg_cmd = doc["msg_cmd"];

                    if(!msg_cmd.IsUint())
                    {
                        break;
                    }

                    message->m_cmd = msg_cmd.GetUint();
                    m_dispatch_cb(std::move(message));
                }
                
                break;
            }
        }
    }
}

}
}
