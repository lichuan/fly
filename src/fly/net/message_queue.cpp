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
 *     fly is an awesome c++ network library.                          *
 *                                                                     *
 *   @author: lichuan                                                  *
 *   @qq: 308831759                                                    *
 *   @email: 308831759@qq.com                                          *
 *   @github: https://github.com/lichuan/fly                           *
 *   @date: 2015-06-22 18:04:48                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "fly/net/message_queue.hpp"

namespace fly {
namespace net {

void Message_Queue::push(Message_Block *message_block)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_queue.push_back(message_block);
    m_length += message_block->length();
}

void Message_Queue::push_front(Message_Block *message_block)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_queue.push_front(message_block);
    m_length += message_block->length();
}

Message_Block* Message_Queue::pop()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    
    if(m_queue.empty())
    {
        return nullptr;
    }
    
    Message_Block* message_block = m_queue.front();
    m_queue.pop_front();
    m_length -= message_block->length();
    
    return message_block;
}

}
}
