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
 *     fly is an awesome c++ network library.                          *
 *                                                                     *
 *   @author: lichuan                                                  *
 *   @qq: 308831759                                                    *
 *   @email: 308831759@qq.com                                          *
 *   @github: https://github.com/lichuan/fly                           *
 *   @date: 2015-06-22 17:35:57                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "fly/net/message_block.hpp"

namespace fly {
namespace net {

Message_Block::Message_Block(uint32 size)
{
    m_data.resize(size + 1, 0);
}

uint32 Message_Block::length()
{
    return m_write_pos - m_read_pos;
}

char* Message_Block::read_ptr()
{
    return &m_data[m_read_pos];
}

void Message_Block::read_ptr(uint32 count)
{
    m_read_pos += count;
}

char* Message_Block::write_ptr()
{
    return &m_data[m_write_pos];
}

void Message_Block::write_ptr(uint32 count)
{
    m_write_pos += count;
}

}
}
