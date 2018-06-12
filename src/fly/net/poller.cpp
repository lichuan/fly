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
 *   @date: 2015-06-23 16:47:24                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "fly/net/poller.hpp"

namespace fly {
namespace net {

template<typename T>
Poller<T>::Poller(uint32 num)
{
    m_scheduler.reset(new fly::task::Scheduler(num));
    m_poller_task_num = num;
    
    for(uint32 i = 1; i <= num; ++i)
    {
        auto *poller_task = new Poller_Task<T>(i);
        m_poller_tasks.push_back(poller_task);
        m_scheduler->schedule_task(poller_task);
    }
}

template<typename T>
void Poller<T>::start()
{
    m_scheduler->start();
}

template<typename T>
void Poller<T>::stop()
{
    for(auto poll_task : m_poller_tasks)
    {
        poll_task->stop();
    }

    m_scheduler->stop();
}

template<typename T>
void Poller<T>::wait()
{
    m_scheduler->wait();
}

template<typename T>
bool Poller<T>::register_connection(std::shared_ptr<Connection<T>> connection)
{
    return m_poller_tasks[connection->id() % m_poller_task_num]->register_connection(connection);
}

template class Poller<Json>;
template class Poller<Wsock>;
template class Poller<Proto>;

}
}
