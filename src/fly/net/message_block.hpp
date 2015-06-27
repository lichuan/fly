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
 *   @date: 2015-06-22 17:19:02                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__NET__MESSAGE_BLOCK
#define FLY__NET__MESSAGE_BLOCK

#include <vector>
#include "fly/base/common.hpp"

namespace fly {
namespace net {

class Message_Block
{
public:
    Message_Block(uint32 size);
    char* read_ptr();
    void read_ptr(uint32 count);
    char* write_ptr();
    void write_ptr(uint32 count);
    uint32 length();
    
private:
    std::vector<char> m_data;
    uint32 m_write_pos = 0;
    uint32 m_read_pos = 0;
};

}
}

#endif
