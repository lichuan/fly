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
#include "fly/net/connection.hpp"
#include "fly/net/poller_task.hpp"
#include "fly/base/logger.hpp"

namespace fly {
namespace net {

fly::base::ID_Allocator Connection::m_id_allocator;

Connection::~Connection()
{
    while(auto *message_block = m_recv_msg_queue.pop())
    {
        delete message_block;
    }

    while(auto *message_block = m_send_msg_queue.pop())
    {
        delete message_block;
    }
}

Connection::Connection(int32 fd, const Addr &peer_addr)
{
    m_fd = fd;
    m_peer_addr = peer_addr;
    m_id = m_id_allocator.new_id();
}

uint64 Connection::id()
{
    return m_id;
}

void Connection::send(void *data, uint32 size)
{
    Message_Block *message_block = new Message_Block(size);
    memcpy(message_block->read_ptr(), data, size);
    message_block->write_ptr(size);
    m_poller_task->write_connection(shared_from_this());
}

void Connection::close()
{
    m_poller_task->close_connection(shared_from_this());
}

void Connection::parse()
{
}

const Addr& Connection::peer_addr()
{
    return m_peer_addr;
}

void Connection::holder(Holder *_holder)
{
    m_holder = _holder;
}

}
}
