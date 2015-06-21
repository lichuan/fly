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
 *   @date: 2015-06-10 13:33:47                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "fly/task/executor.hpp"

namespace fly {
namespace task {

Executor::Executor()
{
}

void Executor::run()
{
    while(true)
    {
        auto *task = m_tasks.pop();
        task->run();
        delete task;
    }
}

void Executor::push_task(Task *task)
{
    m_tasks.push(task);
}

void Executor::start()
{
    std::thread tmp(std::bind(&Executor::run, this));
    m_thread = std::move(tmp);
}

void Executor::stop()
{
    m_thread.join();
}

}
}
