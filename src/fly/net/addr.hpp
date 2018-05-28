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
 *   @date: 2015-06-22 17:03:35                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__NET__ADDR
#define FLY__NET__ADDR

#include <string>
#include "fly/base/common.hpp"

namespace fly {
namespace net {

struct Addr
{
    std::string m_host;
    uint16 m_port;

    Addr() = default;

    Addr(const std::string &host, uint16 port)
    {
        m_host = host;
        m_port = port;
    }

    bool operator==(const Addr &other)
    {
        return m_host == other.m_host && m_port == other.m_port;
    }
};

}
}

#endif
