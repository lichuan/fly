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

using namespace std;

int main()
{
    //init library
    fly::init();

    //init logger
    fly::base::Logger::instance()->init(fly::base::DEBUG, "myapp", "./log/");
    
    //test logger
    for(int32 i = 0; i < 20000; ++i)
    {
        LOG_INFO("this is a msg to logger, I am %s, 1024 * 1024 = %d", "lichuan", 1024 * 1024);
    }
    
    cout << "test end" << endl;
}
