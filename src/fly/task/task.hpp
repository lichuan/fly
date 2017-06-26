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
 *   @date: 2015-06-21 18:45:20                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__TASK__TASK
#define FLY__TASK__TASK

#include "fly/base/common.hpp"

namespace fly {
namespace task {

class Task
{
    friend class Executor;
    friend class Scheduler;
    
public:
    Task(uint64 seq);
    virtual ~Task() = default;
    virtual void run() {}
    uint64 seq();
    void set_executor_id(uint32 id);
    
protected:
    uint32 m_executor_id;
    
private:
    uint64 m_seq;
    bool m_stop_executor = false;
};

}
}

#endif
