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
 *   @date: 2015-06-10 13:32:56                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__BASE__BLOCK_QUEUE
#define FLY__BASE__BLOCK_QUEUE

#include <mutex>
#include <list>
#include <condition_variable>
#include "fly/base/common.hpp"

namespace fly {
namespace base {

template<typename T, uint32 MAX_SIZE = 100>
class Block_Queue
{
public:
    void push(T element)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        bool empty = m_queue.empty();
        m_queue.push_back(element);
        
        if(empty)
        {
            m_cond_not_empty.notify_one();
        }
        else if(m_queue.size() >= MAX_SIZE)
        {
            m_full = true;
            m_cond_not_full.wait(locker, [&]{return !m_full;});
        }
    }
    
    T pop()
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_cond_not_empty.wait(locker, [&]{return !m_queue.empty();});
        T element = m_queue.front();
        m_queue.pop_front();

        if(m_full && m_queue.size() <= MAX_SIZE / 4)
        {
            m_full = false;
            m_cond_not_full.notify_one();
        }
        
        return element;
    }
    
    std::list<T> m_queue;
    std::mutex m_mutex;
    bool m_full = false;
    std::condition_variable m_cond_not_empty;
    std::condition_variable m_cond_not_full;
};

}
}

#endif
