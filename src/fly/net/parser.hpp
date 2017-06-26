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
 *   @date: 2015-06-24 20:45:54                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__NET__PARSER
#define FLY__NET__PARSER

#include <vector>
#include "fly/task/scheduler.hpp"
#include "fly/net/parser_task.hpp"

namespace fly {
namespace net {

class Parser
{
public:
    Parser(uint32 num);
    void wait();
    void start();
    void stop();
    void register_connection(std::shared_ptr<Connection> connection);
    
private:
    std::unique_ptr<fly::task::Scheduler> m_scheduler;
    std::vector<Parser_Task*> m_parser_tasks;
};

}
}

#endif
