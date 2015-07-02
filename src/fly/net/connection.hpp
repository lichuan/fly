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
 *   @date: 2015-06-22 17:15:25                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__NET__CONNECTION
#define FLY__NET__CONNECTION

#include <memory>
#include "fly/net/addr.hpp"
#include "fly/net/message_block_queue.hpp"

namespace fly {
namespace net {

class Poller_Task;
class Parser_Task;
class Server;
class Message_Pack;

class Connection : public std::enable_shared_from_this<Connection>
{
    friend class Parser;
    friend class Poller_Task;
    friend class Parser_Task;
    friend class Server;
    friend class Client;
    
public:
    Connection(int32 fd, const Addr &peer_addr);
    ~Connection();
    uint64 id();
    void close();
    void send(void *data, uint32 size);
    const Addr& peer_addr();
    
private:
    void parse();
    int32 m_fd;
    uint64 m_id = 0;
    Addr m_peer_addr;
    std::atomic<bool> m_closed {false};
    Message_Block_Queue m_recv_msg_queue;
    Message_Block_Queue m_send_msg_queue;
    Poller_Task *m_poller_task = nullptr;
    Parser_Task *m_parser_task = nullptr;
    static fly::base::ID_Allocator m_id_allocator;
    std::function<void(std::shared_ptr<Connection>)> m_close_cb;
    std::function<void(std::shared_ptr<Connection>)> m_be_closed_cb;
    std::function<void(std::shared_ptr<Connection>)> m_init_cb;
    std::function<void(std::unique_ptr<Message_Pack>)> m_dispatch_cb;
};

}
}

#endif
