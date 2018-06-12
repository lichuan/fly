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
 *   @date: 2015-06-23 16:41:24                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__NET__POLLER
#define FLY__NET__POLLER

#include <vector>
#include "fly/task/scheduler.hpp"
#include "fly/net/poller_task.hpp"

namespace fly {
namespace net {

template<typename T>
class Poller
{
public:
    Poller(uint32 num);
    void wait();
    void start();
    void stop();
    bool register_connection(std::shared_ptr<Connection<T>> connection);
    
private:
    std::unique_ptr<fly::task::Scheduler> m_scheduler;
    std::vector<Poller_Task<T>*> m_poller_tasks;
    uint32 m_poller_task_num = 0;
};

}
}

#endif
