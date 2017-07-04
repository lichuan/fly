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

template<typename T>
Parser<T>::Parser(uint32 num)
{
    m_scheduler.reset(new fly::task::Scheduler(num));
    m_parser_task_num = num;

    for(uint32 i = 1; i <= num; ++i)
    {
        auto *parser_task = new Parser_Task<T>(i);
        m_parser_tasks.push_back(parser_task);
        m_scheduler->schedule_task(parser_task);
    }
}

template<typename T>
void Parser<T>::start()
{
    m_scheduler->start();
}

template<typename T>
void Parser<T>::stop()
{
    for(auto parser_task : m_parser_tasks)
    {
        auto connection = std::make_shared<Connection<T>>(-1, Addr("stop_parse", 0));
        connection->m_stop_parse = true;
        parser_task->push_connection(connection);
    }

    m_scheduler->stop();
}

template<typename T>
void Parser<T>::wait()
{
    m_scheduler->wait();
}

template<typename T>
void Parser<T>::register_connection(std::shared_ptr<Connection<T>> connection)
{
    connection->m_parser_task = m_parser_tasks[connection->id() % m_parser_task_num];
}

template class Parser<Json>;
template class Parser<Wsock>;
template class Parser<Proto>;

}
}
