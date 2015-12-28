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
 *   @date: 2015-06-10 13:33:17                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <cstdint>
#include <atomic>

#ifndef FLY__BASE__COMMON
#define FLY__BASE__COMMON

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

namespace fly {
namespace base {

template<typename T>
class Guard_Value
{
public:
    Guard_Value(T &value, T in_value, T out_value) : m_value(value)
    {
        m_value = in_value;
        m_out_value = out_value;
    }
    
    ~Guard_Value()
    {
        m_value = m_out_value;
    }

    T &m_value;
    T m_out_value;
};

class ID_Allocator
{
public:
    uint64 new_id();

private:
    std::atomic<uint64> m_id {1};
};

uint32 random_between(uint32 min, uint32 max);
uint32 random_32();
uint64 random_64();
bool rate_by_percent(uint32 rate);    
bool rate_by_thousand(uint32 rate);

}
}

#endif
