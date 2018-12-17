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
 *   @date: 2015-06-10 13:33:25                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__BASE__LOCK_QUEUE
#define FLY__BASE__LOCK_QUEUE

#include <mutex>
#include <list>
#include <condition_variable>

namespace fly {
namespace base {

template<typename T, uint32 MAX_SIZE = 1000>
class Lock_Queue
{
public:
    void push(T element)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_queue.push_back(element);

        if(m_queue.size() >= MAX_SIZE)
        {
            m_full = true;
            m_cond_not_full.wait(locker, [&]{return !m_full;});
        }
    }

    void push_direct(T element)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_queue.push_back(element);
    }
    
    bool pop(std::list<T> &queue)
    {
        std::unique_lock<std::mutex> locker(m_mutex);

        if(m_queue.empty())
        {
            return false;
        }
        
        queue.swap(m_queue);
        
        if(m_full)
        {
            m_full = false;
            m_cond_not_full.notify_one();
        }

        return true;
    }

    void pulse_notify_not_full()
    {
        m_full = false;
        m_cond_not_full.notify_one();
    }

private:
    std::list<T> m_queue;
    std::mutex m_mutex;
    bool m_full = false;
    std::condition_variable m_cond_not_full;
};

template<typename T, uint32 MAX_SIZE>
class Lock_Queue<std::unique_ptr<T>, MAX_SIZE>
{
public:
    void push(std::unique_ptr<T> element)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_queue.push_back(std::move(element));
        
        if(m_queue.size() >= MAX_SIZE)
        {
            m_full = true;
            m_cond_not_full.wait(locker, [&]{return !m_full;});
        }
    }

    void push_direct(std::unique_ptr<T> element)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_queue.push_back(std::move(element));
    }
    
    bool pop(std::list<std::unique_ptr<T>> &queue)
    {
        std::unique_lock<std::mutex> locker(m_mutex);

        if(m_queue.empty())
        {
            return false;
        }
        
        queue.swap(m_queue);
        
        if(m_full)
        {
            m_full = false;
            m_cond_not_full.notify_one();
        }

        return true;
    }
    
private:
    std::list<std::unique_ptr<T>> m_queue;
    std::mutex m_mutex;
    bool m_full = false;
    std::condition_variable m_cond_not_full;
};

}
}

#endif
