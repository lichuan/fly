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
 *   @date: 2015-06-24 20:50:18                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "fly/net/parser_task.hpp"

namespace fly {
namespace net {

template<typename T>
Parser_Task<T>::Parser_Task(uint64 seq) : Task(seq)
{
}

template<typename T>
void Parser_Task<T>::push_connection(std::shared_ptr<Connection<T>> connection)
{
    m_queue.push(connection);
}

template<typename T>
void Parser_Task<T>::run()
{
    while(std::shared_ptr<Connection<T>> connection = m_queue.pop())
    {
        if(connection->m_stop_parse)
        {
            break;
        }

        connection->parse();
    }
}

template class Parser_Task<Json>;
template class Parser_Task<Wsock>;
template class Parser_Task<Proto>;

}
}
