#ifndef FLY__TASK__STOP_EXECUTOR_TASK
#define FLY__TASK__STOP_EXECUTOR_TASK

#include "fly/task/task.hpp"

namespace fly {
namespace task {

class Stop_Executor_Task : public Task
{
public:
    Stop_Executor_Task(uint64 seq = 0) : Task(seq) {}
    virtual void run() override {};
    virtual bool stop_executor() {return true;}
};

}
}

#endif
