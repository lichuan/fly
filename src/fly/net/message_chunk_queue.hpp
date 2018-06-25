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
 *   @date: 2015-06-22 17:48:41                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__NET__MESSAGE_CHUNK_QUEUE
#define FLY__NET__MESSAGE_CHUNK_QUEUE

#include <mutex>
#include <list>
#include "fly/net/message_chunk.hpp"

namespace fly {
namespace net {

class Message_Chunk_Queue
{
public:
    void push(Message_Chunk *message_chunk);    
    void push_front(Message_Chunk *message_chunk);
    Message_Chunk* pop();
    uint32 length();
    
private:
    std::list<Message_Chunk*> m_queue;
    std::mutex m_mutex;
    uint32 m_length = 0;
};

}
}

#endif
