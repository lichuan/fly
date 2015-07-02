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
 *   @date: 2015-06-24 20:46:52                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "fly/net/parser.hpp"

namespace fly {
namespace net {

Parser::Parser(uint32 num)
{
    m_scheduler.reset(new fly::task::Scheduler(num));
    m_scheduler->start();
    
    for(uint32 i = 0; i < num; ++i)
    {
        auto *parser_task = new Parser_Task(i);
        m_parser_tasks.push_back(parser_task);
        m_scheduler->schedule_task(parser_task);
    }

    m_num = num;
}

void Parser::wait()
{
    m_scheduler->wait();

    for(auto parser_task : m_parser_tasks)
    {
        delete parser_task;
    }
}

void Parser::register_connection(std::shared_ptr<Connection> connection)
{
    connection->m_parser_task = m_parser_tasks[connection->id() % m_num];
}

}
}
