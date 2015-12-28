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
 *   @email: lichuan@lichuan.me                                        *
 *   @github: https://github.com/lichuan/fly                           *
 *   @date: 2015-06-23 16:47:24                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "fly/net/poller.hpp"

namespace fly {
namespace net {

Poller::Poller(uint32 num)
{
    m_scheduler.reset(new fly::task::Scheduler(num));
    
    for(uint32 i = 0; i < num; ++i)
    {
        auto *poller_task = new Poller_Task(i);
        m_poller_tasks.push_back(poller_task);
        m_scheduler->schedule_task(poller_task);
    }
}

void Poller::start()
{
    m_scheduler->start();
}

void Poller::wait()
{
    m_scheduler->wait();

    for(auto poll_task : m_poller_tasks)
    {
        delete poll_task;
    }
}

void Poller::register_connection(std::shared_ptr<Connection> connection)
{
    static auto num = m_poller_tasks.size();
    m_poller_tasks[connection->id() % num]->register_connection(connection);
}

}
}
