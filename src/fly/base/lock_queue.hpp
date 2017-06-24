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
#include <vector>

namespace fly {
namespace base {

template<typename T>
class Lock_Queue
{
public:
    void push(T element)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_queue.push_back(element);
    }
    
    bool pop(std::vector<T> &queue)
    {
        std::lock_guard<std::mutex> guard(m_mutex);

        if(m_queue.empty())
        {
            return false;
        }

        queue.swap(m_queue);

        return true;
    }
    
private:
    std::vector<T> m_queue;
    std::mutex m_mutex;
};

// template<typename T>
// class Lock_Queue<T*>
// {
// public:
//     void push(T *element)
//     {
//         std::lock_guard<std::mutex> guard(m_mutex);
//         m_queue.push_back(element);
//     }

//     T* pop()
//     {
//         std::lock_guard<std::mutex> guard(m_mutex);
        
//         if(m_queue.empty())
//         {
//             return nullptr;
//         }
        
//         T *element = m_queue.front();
//         m_queue.pop_front();
        
//         return element;
//     }
    
// private:
//     std::deque<T*> m_queue;
//     std::mutex m_mutex;
// };

}
}

#endif
