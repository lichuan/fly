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
 *   @date: 2015-06-10 13:34:21                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include <unistd.h>
#include "fly/init.hpp"
#include "fly/base/logger.hpp"
#include "fly/base/block_queue.hpp"
#include "fly/base/lock_queue.hpp"
#include "fly/task/scheduler.hpp"

using namespace std;

class Print_Task : public fly::task::Loop_Task
{
public:
    Print_Task(uint64 seq) : Loop_Task(seq)
    {        
    }

    virtual void run_in_loop() override
    {
        cout << "Print_Task::run_in_loop." << endl;
        sleep(1);
    }
};

int main()
{
    //init library
    fly::init();

    //init logger
    fly::base::Logger::instance()->init(fly::base::DEBUG, "myapp", "./log/");
    
    //test logger
    LOG_INFO("this is a msg to logger, I am %s, 1024 * 1024 = %d", "lichuan", 1024 * 1024);
    
    //test task
    fly::task::Scheduler scheduler(1);
    scheduler.schedule_task(new Print_Task(0));
    scheduler.start();
    scheduler.stop();
    cout << "test end" << endl;
}
