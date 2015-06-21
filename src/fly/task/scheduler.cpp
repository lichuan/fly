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
 *   @date: 2015-06-21 19:27:31                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "fly/task/scheduler.hpp"

namespace fly {
namespace task {

Scheduler::Scheduler(uint32 num)
{
    for(uint32 i = 0; i < num; ++i)
    {
        m_executors.push_back(new Executor);
    }
}

void Scheduler::start()
{
    for(auto *executor : m_executors)
    {
        executor->start();
    }
}

void Scheduler::stop()
{
    for(auto *executor : m_executors)
    {
        executor->stop();
        delete executor;
    }
}

void Scheduler::schedule_task(Task *task)
{
    uint64 seq = task->seq();
    static auto executor_num = m_executors.size();
    auto i = 0;
    
    if(seq == 0) //select executor randomly
    {
        i = fly::base::random_between(0, executor_num - 1);
    }
    else //select executor by sequence number
    {
        i = task->seq() % executor_num;
    }

    auto *executor = m_executors[i];
    task->set_executor_id(i);
    executor->push_task(task);
}

}
}
