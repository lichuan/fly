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
 *   @date: 2015-06-22 18:04:48                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "fly/net/message_chunk_queue.hpp"

namespace fly {
namespace net {

void Message_Chunk_Queue::push(Message_Chunk *message_chunk)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_queue.push_back(message_chunk);
    m_length += message_chunk->length();
}

void Message_Chunk_Queue::push_front(Message_Chunk *message_chunk)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_queue.push_front(message_chunk);
    m_length += message_chunk->length();
}

uint32 Message_Chunk_Queue::length()
{
    return m_length;
}

Message_Chunk* Message_Chunk_Queue::pop()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    
    if(m_queue.empty())
    {
        return nullptr;
    }
    
    Message_Chunk* message_chunk = m_queue.front();
    m_queue.pop_front();
    m_length -= message_chunk->length();
    
    return message_chunk;
}

}
}
