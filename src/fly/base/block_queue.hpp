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
 *   @email: lichuan@lichuan.me                                        *
 *   @github: https://github.com/lichuan/fly                           *
 *   @date: 2015-06-10 13:32:56                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__BASE__BLOCK_QUEUE
#define FLY__BASE__BLOCK_QUEUE

#include <mutex>
#include <deque>
#include <condition_variable>

namespace fly {
namespace base {

template<typename T>
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
            locker.unlock();
            m_cond.notify_one();
        }
    }
    
    T pop()
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_cond.wait(locker, [&]{return !m_queue.empty();});
        T element = m_queue.front();
        m_queue.pop_front();

        return element;
    }
    
    std::deque<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};

}
}

#endif
