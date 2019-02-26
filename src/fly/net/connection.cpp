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
#include "rapidjson/error/en.h"
#include <thread>

namespace fly {
namespace net {

//Json
fly::base::ID_Allocator Connection<Json>::m_id_allocator;

Connection<Json>::~Connection()
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

Connection<Json>::Connection(int32 fd, const Addr &peer_addr)
{
    m_fd = fd;
    m_peer_addr = peer_addr;
    m_is_passive = true;
}

bool Connection<Json>::is_passive()
{
    return m_is_passive;
}

std::string Connection<Json>::key() const
{
    return m_key;
}

void Connection<Json>::key(std::string k)
{
    m_key = k;
}

void Connection<Json>::set_passive(bool is_passive)
{
    m_is_passive = is_passive;
}

uint64 Connection<Json>::id()
{
    return m_id;
}

void Connection<Json>::send(rapidjson::Document &doc)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    send(buffer.GetString(), buffer.GetSize());
}

void Connection<Json>::send(const void *data, uint32 size)
{
    Message_Chunk *message_chunk = new Message_Chunk(size + sizeof(uint32));
    uint32 *uint32_ptr = (uint32*)message_chunk->read_ptr();
    *uint32_ptr = htonl(size);
    memcpy(message_chunk->read_ptr() + sizeof(uint32), data, size);
    message_chunk->write_ptr(size + sizeof(uint32));
    m_send_msg_queue.push(message_chunk);
    m_poller_task->write_connection(shared_from_this());
}

void Connection<Json>::close()
{
    m_poller_task->close_connection(shared_from_this());
}

bool Connection<Json>::closed()
{
    return m_closed.load(std::memory_order_relaxed);
}

const Addr& Connection<Json>::peer_addr()
{
    return m_peer_addr;
}

void Connection<Json>::parse()
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
        if(m_cur_msg_length > m_max_msg_length)
        {
            LOG_DEBUG_ERROR("json message length(%lu) exceed max_msg_length(%u) from %s:%u", m_cur_msg_length, m_max_msg_length, \
                      m_peer_addr.m_host.c_str(), m_peer_addr.m_port);
            close();
            return;
        }
        
        if(m_recv_msg_queue.length() < m_cur_msg_length)
        {
            return;
        }
        
        const uint32 MAX_STACK_SIZE = 512 * 1024;
        char msg_buf[MAX_STACK_SIZE] = {0};
        char *data = msg_buf;
        bool is_new_buf = false;
        remain_bytes = m_cur_msg_length;
        
        if(m_cur_msg_length > MAX_STACK_SIZE)
        {
            data = new char[m_cur_msg_length];
            is_new_buf = true;
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

                std::unique_ptr<Message<Json>> message(new Message<Json>(shared_from_this()));
                message->m_raw_data.assign(data, m_cur_msg_length);
                message->m_length = m_cur_msg_length;
                m_cur_msg_length = 0;

                if(is_new_buf)
                {
                    delete[] data;
                }
                
                rapidjson::Document &doc = message->doc();
                doc.Parse(message->m_raw_data.c_str());

                if(doc.HasParseError())
                {
                    LOG_DEBUG_ERROR("parse json message failed from %s:%u, reason: %s", m_peer_addr.m_host.c_str(), m_peer_addr.m_port, \
                                    GetParseError_En(doc.GetParseError()));
                    close();
                    return;
                }

                if(!doc.IsObject())
                {
                    close();
                    return;
                }
                
                if(!doc.HasMember("msg_type"))
                {
                    close();
                    return;
                }
                    
                const rapidjson::Value &msg_type = doc["msg_type"];

                if(!msg_type.IsUint())
                {
                    close();
                    return;
                }
                    
                message->m_type = msg_type.GetUint();

                if(!doc.HasMember("msg_cmd"))
                {
                    close();
                    return;
                }
                    
                const rapidjson::Value &msg_cmd = doc["msg_cmd"];

                if(!msg_cmd.IsUint())
                {
                    close();
                    return;
                }
                    
                message->m_cmd = msg_cmd.GetUint();
                m_dispatch_cb(std::move(message));
                
                break;
            }
        }
    }
}

//Wsock
fly::base::ID_Allocator Connection<Wsock>::m_id_allocator;

Connection<Wsock>::~Connection()
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

Connection<Wsock>::Connection(int32 fd, const Addr &peer_addr)
{
    m_fd = fd;
    m_peer_addr = peer_addr;
    m_is_passive = true;
}

bool Connection<Wsock>::is_passive()
{
    return m_is_passive;
}

void Connection<Wsock>::set_passive(bool is_passive)
{
    m_is_passive = is_passive;
}

uint64 Connection<Wsock>::id()
{
    return m_id;
}

std::string Connection<Wsock>::key() const
{
    return m_key;
}

void Connection<Wsock>::key(std::string k)
{
    m_key = k;
}

void Connection<Wsock>::send(rapidjson::Document &doc)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    send(buffer.GetString(), buffer.GetSize());
}

void Connection<Wsock>::send_raw(const void *data, uint32 size)
{
    Message_Chunk *message_chunk = new Message_Chunk(size);
    memcpy(message_chunk->read_ptr(), data, size);
    message_chunk->write_ptr(size);
    m_send_msg_queue.push(message_chunk);
    m_poller_task->write_connection(shared_from_this());
}

void Connection<Wsock>::send(const void *data, uint32 size)
{
    //assemble websocket packet
    char *buf, *p_data;
    Message_Chunk *message_chunk;
    
    if(size > 0xffff)
    {
        message_chunk = new Message_Chunk(size + 10);
        message_chunk->write_ptr(size + 10);
        buf = message_chunk->read_ptr();
        buf[1] = 127;
        uint64 *p_length = (uint64*)(buf + 2);
        *p_length = fly::base::htonll(size);
        p_data = buf + 10;
    }
    else if(size > 125)
    {
        message_chunk = new Message_Chunk(size + 4);
        message_chunk->write_ptr(size + 4);
        buf = message_chunk->read_ptr();
        buf[1] = 126;
        uint16 *p_length = (uint16*)(buf + 2);
        *p_length = htons(size);
        p_data = buf + 4;
    }
    else
    {
        message_chunk = new Message_Chunk(size + 2);
        message_chunk->write_ptr(size + 2);
        buf = message_chunk->read_ptr();
        buf[1] = size;
        p_data = buf + 2;
    }
    
    buf[0] = 0x81;
    memcpy(p_data, data, size);
    m_send_msg_queue.push(message_chunk);
    m_poller_task->write_connection(shared_from_this());
}

void Connection<Wsock>::close()
{
    //base::crash_me();
    m_poller_task->close_connection(shared_from_this());
}

bool Connection<Wsock>::closed()
{
    return m_closed.load(std::memory_order_relaxed);
}

const Addr& Connection<Wsock>::peer_addr()
{
    return m_peer_addr;
}

void Connection<Wsock>::parse()
{
    if(m_handshake_phase)
    {
        std::string req;
        std::list<Message_Chunk*> chunks;
        fly::base::Scope_CB scope_cb(
            [&chunks, this]
            {
                while(true)
                {
                    if(chunks.empty())
                    {
                        break;
                    }
                    
                    auto *message_chunk = chunks.front();
                    chunks.pop_front();
                    m_recv_msg_queue.push_front(message_chunk);
                }
            },
            [&chunks, this]
            {
                while(true)
                {
                    if(chunks.empty())
                    {
                        break;
                    }

                    auto *message_chunk = chunks.front();
                    chunks.pop_front();
                    delete message_chunk;
                }
            });
        
        while(auto *message_chunk = m_recv_msg_queue.pop())
        {
            char *chunk_ptr = message_chunk->read_ptr();
            req += chunk_ptr;
            chunks.push_front(message_chunk);
        }
        
        uint32 len = req.length();
        
        //too short for wsock handshake packet
        if(len < 80)
        {
            return;
        }

        if(req[len-4] != '\r' || req[len - 3] != '\n' || req[len - 2] != '\r' || req[len - 1] != '\n')
        {
            return;
        }
        
        std::string::size_type key_pos = req.find("Sec-WebSocket-Key: ");
        scope_cb.set_cur_cb(1);

        if(key_pos == std::string::npos)
        {
            close();
            return;
        }

        std::string key_str = req.substr(key_pos + 19, (req.find("\r\n", key_pos + 19) - (key_pos + 19)));
        std::string rsp = "HTTP/1.1 101 Switching Protocols\r\n";
        rsp += "Connection: Upgrade\r\n";
        rsp += "Upgrade: websocket\r\n";
        //rsp += "Sec-WebSocket-Protocol: sub-protocol\r\n";
        rsp += "Sec-WebSocket-Accept: ";
        const static std::string wsock_magic_key("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
        std::string server_key = key_str + wsock_magic_key;
        char sha1_buf[20] = {0};
        fly::base::sha1(server_key.c_str(), server_key.length(), sha1_buf, 20);
        rsp += fly::base::base64_encode(sha1_buf, 20);
        rsp += "\r\n\r\n";
        send_raw(rsp.c_str(), rsp.length());
        m_handshake_phase = false;
        return;
    }

    while(true)
    {
        if(m_cur_msg_length != 0)
        {
            if(m_cur_msg_length_1 != 0)
            {
                goto after_parse_length_1;
            }
            
            goto after_parse_length;
        }
        
        if(m_recv_msg_queue.length() < 2)
        {
            return;
        }

        {
            char buf[2] = {0};
            auto *message_chunk = m_recv_msg_queue.pop();

            if(message_chunk->length() >= 2)
            {
                memcpy(buf, message_chunk->read_ptr(), 2);
                message_chunk->read_ptr(2);
            }
            else
            {
                memcpy(buf, message_chunk->read_ptr(), 1);
                delete message_chunk;
                message_chunk = m_recv_msg_queue.pop();
                memcpy(buf, message_chunk->read_ptr(), 1);
                message_chunk->read_ptr(1);
            }

            if(message_chunk->length() == 0)
            {
                delete message_chunk;
            }
            else
            {
                m_recv_msg_queue.push_front(message_chunk);
            }

            uint8 fin = buf[0] >> 7;

            if(fin == 0)
            {
                LOG_DEBUG_ERROR("recv websocket but fin == 0 from %s:%u", m_peer_addr.m_host.c_str(), m_peer_addr.m_port);
                close();
                return;
            }

            if((buf[0] & 0x70) != 0)
            {
                LOG_DEBUG_ERROR("recv websocket but (buf[0] & 0x70) != 0 from %s:%u", m_peer_addr.m_host.c_str(), m_peer_addr.m_port);
                close();
                return;
            }
        
            uint8 op_code = buf[0] & 0x0f;
            bool is_ping_packet = false;

            if(op_code == 0x01) //text frame
            {
            }
            else if(op_code == 0x08) //close
            {
                LOG_DEBUG_INFO("recv websocket close protocol from %s:%u", m_peer_addr.m_host.c_str(), m_peer_addr.m_port);
                close();
                return;
            }
            else if(op_code == 0x09) //ping
            {
                LOG_DEBUG_INFO("recv websocket ping protocol from %s:%u", m_peer_addr.m_host.c_str(), m_peer_addr.m_port);
                char *buf, *p_data;
                Message_Chunk *message_chunk = new Message_Chunk(2);
                message_chunk->write_ptr(2);
                buf = message_chunk->read_ptr();
                buf[1] = 0;
                buf[0] = 0x8a;
                m_send_msg_queue.push(message_chunk);
                m_poller_task->write_connection(shared_from_this());
                is_ping_packet = true;
            }
            else if(op_code == 0x0a) //pong
            {
                LOG_DEBUG_ERROR("recv websocket pong protocol from %s:%u", m_peer_addr.m_host.c_str(), m_peer_addr.m_port);
                close();
                return;
            }
            else
            {
                LOG_DEBUG_ERROR("recv websocket other protocol from %s:%u", m_peer_addr.m_host.c_str(), m_peer_addr.m_port);
                close();
                return;
            }
            
            if((buf[1] & 0x80) == 0)
            {
                LOG_DEBUG_ERROR("recv websocket but (buf[1] & 0x80) == 0 buf[1]: %u from %s:%u", buf[1], m_peer_addr.m_host.c_str(), m_peer_addr.m_port);
                close();
                return;
            }
            
            m_cur_msg_length = buf[1] & 0x7f;
            
            if(m_cur_msg_length == 0)
            {
                if(!is_ping_packet)
                {
                    LOG_DEBUG_ERROR("recv websocket but (buf[1] & 0x7f) == 0 from %s:%u", m_peer_addr.m_host.c_str(), m_peer_addr.m_port);
                    close();
                    return;
                }
            }
        }
        
    after_parse_length:
        if(m_cur_msg_length == 126)
        {
            if(m_recv_msg_queue.length() < 2)
            {
                return;
            }
            else
            {
                uint16 msg_length = 0;
                auto *message_chunk = m_recv_msg_queue.pop();

                if(message_chunk->length() >= 2)
                {
                    memcpy(&msg_length, message_chunk->read_ptr(), 2);
                    message_chunk->read_ptr(2);
                }
                else
                {
                    memcpy(&msg_length, message_chunk->read_ptr(), 1);
                    delete message_chunk;
                    message_chunk = m_recv_msg_queue.pop();
                    memcpy(&msg_length, message_chunk->read_ptr(), 1);
                    message_chunk->read_ptr(1);
                }

                if(message_chunk->length() == 0)
                {
                    delete message_chunk;
                }
                else
                {
                    m_recv_msg_queue.push_front(message_chunk);
                }
                
                m_cur_msg_length_1 = ntohs(msg_length);

                if(m_cur_msg_length_1 == 0)
                {
                    close();
                    return;
                }
            }
        }
        else if(m_cur_msg_length == 127)
        {
            if(m_recv_msg_queue.length() < 8)
            {
                return;
            }
            else
            {
                uint64 msg_length = 0;
                uint32 remain_bytes = 8;
                char *msg_length_buf = (char*)(&msg_length);
                
                while(auto *message_chunk = m_recv_msg_queue.pop())
                {
                    uint32 length = message_chunk->length();
            
                    if(length < remain_bytes)
                    {
                        memcpy(msg_length_buf + sizeof(uint64) - remain_bytes, message_chunk->read_ptr(), length);
                        remain_bytes -= length;
                        delete message_chunk;
                    }
                    else
                    {
                        memcpy(msg_length_buf + sizeof(uint64) - remain_bytes, message_chunk->read_ptr(), remain_bytes);

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

                m_cur_msg_length_1 = fly::base::ntohll(msg_length);
                
                if(m_cur_msg_length_1 == 0)
                {
                    close();
                    return;
                }
            }
        }
        
    after_parse_length_1:
        uint64 msg_length = 0;

        if(m_cur_msg_length < 126)
        {
            msg_length = m_cur_msg_length;
        }
        else
        {
            msg_length = m_cur_msg_length_1;
        }

        if(msg_length > m_max_msg_length)
        {
            LOG_DEBUG_ERROR("wsock message length(%lu) exceed max_msg_length(%u) from %s:%u", msg_length, m_max_msg_length, m_peer_addr.m_host.c_str(), m_peer_addr.m_port);
            close();
            return;
        }
        
        //4 bytes mask
        if(m_recv_msg_queue.length() < msg_length + 4)
        {
            return;
        }
        
        char mask_keys[4] = {0};
        uint32 remain_bytes = 4;

        while(auto *message_chunk = m_recv_msg_queue.pop())
        {
            uint32 length = message_chunk->length();
            
            if(length < remain_bytes)
            {
                memcpy(mask_keys + 4 - remain_bytes, message_chunk->read_ptr(), length);
                remain_bytes -= length;
                delete message_chunk;
            }
            else
            {
                memcpy(mask_keys + 4 - remain_bytes, message_chunk->read_ptr(), remain_bytes);

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

        if(msg_length > 0)
        {
            const uint32 MAX_STACK_SIZE = 512 * 1024;
            char msg_buf[MAX_STACK_SIZE] = {0};
            char *data = msg_buf;
            bool is_new_buf = false;
            remain_bytes = msg_length;
        
            if(msg_length > MAX_STACK_SIZE)
            {
                data = new char[msg_length];
                is_new_buf = true;
            }
            
            while(auto *message_chunk = m_recv_msg_queue.pop())
            {
                uint32 length = message_chunk->length();

                if(length < remain_bytes)
                {
                    memcpy(data + msg_length - remain_bytes, message_chunk->read_ptr(), length);
                    remain_bytes -= length;
                    delete message_chunk;
                }
                else
                {
                    memcpy(data + msg_length - remain_bytes, message_chunk->read_ptr(), remain_bytes);

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
            
            for(auto i = 0; i < msg_length; ++i)
            {
                data[i] = data[i] ^ mask_keys[i % 4];
            }

            std::unique_ptr<Message<Wsock>> message(new Message<Wsock>(shared_from_this()));
            message->m_raw_data.assign(data, msg_length);
            message->m_length = msg_length;
            msg_length = 0;
        
            if(is_new_buf)
            {
                delete[] data;
            }
        
            rapidjson::Document &doc = message->doc();
            doc.Parse(message->m_raw_data.c_str());
                
            if(doc.HasParseError())
            {
                LOG_DEBUG_ERROR("websocket parse json failed from %s:%u, reason: %s", m_peer_addr.m_host.c_str(), m_peer_addr.m_port, \
                                GetParseError_En(doc.GetParseError()));
                close();
                return;
            }
        
            if(!doc.IsObject())
            {
                close();
                return;
            }
        
            if(!doc.HasMember("msg_type"))
            {
                LOG_DEBUG_ERROR("websocket parse msg_type failed from %s:%u", m_peer_addr.m_host.c_str(), m_peer_addr.m_port);
                close();
                return;
            }
                    
            const rapidjson::Value &msg_type = doc["msg_type"];

            if(!msg_type.IsUint())
            {
                close();
                return;
            }

            message->m_type = msg_type.GetUint();

            if(!doc.HasMember("msg_cmd"))
            {
                LOG_DEBUG_ERROR("websocket parse msg_cmd failed from %s:%u", m_peer_addr.m_host.c_str(), m_peer_addr.m_port);
                close();
                return;
            }
        
            const rapidjson::Value &msg_cmd = doc["msg_cmd"];

            if(!msg_cmd.IsUint())
            {
                close();
                return;
            }
        
            message->m_cmd = msg_cmd.GetUint();
            m_dispatch_cb(std::move(message));
        }
        
        m_cur_msg_length = 0;
        m_cur_msg_length_1 = 0;
    }
}

//Proto
fly::base::ID_Allocator Connection<Proto>::m_id_allocator;

Connection<Proto>::~Connection()
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

Connection<Proto>::Connection(int32 fd, const Addr &peer_addr)
{
    m_fd = fd;
    m_peer_addr = peer_addr;
    m_is_passive = true;
}

bool Connection<Proto>::is_passive()
{
    return m_is_passive;
}

void Connection<Proto>::set_passive(bool is_passive)
{
    m_is_passive = is_passive;
}

std::string Connection<Proto>::key() const
{
    return m_key;
}

void Connection<Proto>::key(std::string k)
{
    m_key = k;
}

uint64 Connection<Proto>::id()
{
    return m_id;
}

void Connection<Proto>::send(rapidjson::Document &doc)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    send(buffer.GetString(), buffer.GetSize());
}

void Connection<Proto>::send(const void *data, uint32 size)
{
    Message_Chunk *message_chunk = new Message_Chunk(size + sizeof(uint32));
    uint32 *uint32_ptr = (uint32*)message_chunk->read_ptr();
    *uint32_ptr = htonl(size);
    memcpy(message_chunk->read_ptr() + sizeof(uint32), data, size);
    message_chunk->write_ptr(size + sizeof(uint32));
    m_send_msg_queue.push(message_chunk);
    m_poller_task->write_connection(shared_from_this());
}

void Connection<Proto>::close()
{
    m_poller_task->close_connection(shared_from_this());
}

bool Connection<Proto>::closed()
{
    return m_closed.load(std::memory_order_relaxed);
}

const Addr& Connection<Proto>::peer_addr()
{
    return m_peer_addr;
}

void Connection<Proto>::parse()
{
    while(true)
    {
        break; // unimplemented !!!
        
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
        if(m_cur_msg_length > m_max_msg_length)
        {
            LOG_DEBUG_ERROR("proto message length(%lu) exceed max_msg_length(%u) from %s:%u", m_cur_msg_length, m_max_msg_length, \
                      m_peer_addr.m_host.c_str(), m_peer_addr.m_port);
            close();
            return;
        }

        if(m_recv_msg_queue.length() < m_cur_msg_length)
        {
            return;
        }
        
        const uint32 MAX_STACK_SIZE = 512 * 1024;
        char msg_buf[MAX_STACK_SIZE] = {0};
        char *data = msg_buf;
        bool is_new_buf = false;
        remain_bytes = m_cur_msg_length;
        
        if(m_cur_msg_length > MAX_STACK_SIZE)
        {
            data = new char[m_cur_msg_length];
            is_new_buf = true;
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

                std::unique_ptr<Message<Proto>> message(new Message<Proto>(shared_from_this()));
                message->m_raw_data.assign(data, m_cur_msg_length);
                message->m_length = m_cur_msg_length;
                m_cur_msg_length = 0;
                
                if(is_new_buf)
                {
                    delete[] data;
                }
                
                rapidjson::Document &doc = message->doc();
                doc.Parse(message->m_raw_data.c_str());

                if(doc.HasParseError())
                {
                    LOG_DEBUG_ERROR("parse json message failed from %s:%u, reason: %s", m_peer_addr.m_host.c_str(), m_peer_addr.m_port, \
                                    GetParseError_En(doc.GetParseError()));
                    close();
                    return;
                }
                
                if(!doc.IsObject())
                {
                    close();
                    return;
                }

                if(!doc.HasMember("msg_type"))
                {
                    close();
                    return;
                }
                    
                const rapidjson::Value &msg_type = doc["msg_type"];

                if(!msg_type.IsUint())
                {
                    close();
                    return;
                }

                message->m_type = msg_type.GetUint();

                if(!doc.HasMember("msg_cmd"))
                {
                    close();
                    return;
                }

                const rapidjson::Value &msg_cmd = doc["msg_cmd"];

                if(!msg_cmd.IsUint())
                {
                    close();
                    return;
                }
                    
                message->m_cmd = msg_cmd.GetUint();
                m_dispatch_cb(std::move(message));

                break;
            }
        }
    }
}

}
}
