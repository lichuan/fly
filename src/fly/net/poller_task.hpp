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
 *   @date: 2015-06-24 20:31:38                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__NET__POLLER_TASK
#define FLY__NET__POLLER_TASK

#include "fly/task/loop_task.hpp"
#include "fly/net/connection.hpp"
#include "fly/base/lock_queue.hpp"

namespace fly {
namespace net {

class Poller_Task : public fly::task::Loop_Task
{
public:
    Poller_Task(uint64 seq);
    void register_connection(std::shared_ptr<Connection> connection);
    virtual void run_in_loop() override;
    void close_connection(std::shared_ptr<Connection> connection);
    void write_connection(std::shared_ptr<Connection> connection);
    void stop();
    
private:
    void do_close();
    void do_write();
    void do_write(std::shared_ptr<Connection> connection);
    int32 m_fd;
    int32 m_close_event_fd;
    int32 m_write_event_fd;
    int32 m_stop_event_fd;
    std::unique_ptr<Connection> m_close_udata;
    std::unique_ptr<Connection> m_write_udata;
    std::unique_ptr<Connection> m_stop_udata;
    fly::base::Lock_Queue<std::shared_ptr<Connection>> m_close_queue;
    fly::base::Lock_Queue<std::shared_ptr<Connection>> m_write_queue;
};

}
}

#endif
