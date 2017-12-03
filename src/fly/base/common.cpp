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
 *   @date: 2015-06-10 13:33:07                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <random>
#include "fly/base/common.hpp"
#include "cryptopp/base64.h"
#include "cryptopp/sha.h"
#include "cryptopp/hex.h"

namespace fly {
namespace base {

Scope_CB::Scope_CB(std::function<void()> cb0, std::function<void()> cb1, std::function<void()> cb2, std::function<void()> cb3, \
                   std::function<void()> cb4, std::function<void()> cb5, std::function<void()> cb6, std::function<void()> cb7)
{
    m_cbs[0] = cb0;
    m_cbs[1] = cb1;
    m_cbs[2] = cb2;
    m_cbs[3] = cb3;
    m_cbs[4] = cb4;
    m_cbs[5] = cb5;
    m_cbs[6] = cb6;
    m_cbs[7] = cb7;
}

Scope_CB::~Scope_CB()
{
    m_cbs[m_index]();
}

void Scope_CB::set_cur_cb(uint8 index)
{
    if(index > 7)
    {
        return;
    }

    m_index = index;
}

uint64 ID_Allocator::new_id()
{
    if(m_id.load(std::memory_order_relaxed) >= 18446744073709551615UL)
    {
        m_id.store(1, std::memory_order_relaxed);
    }
    
    return m_id.fetch_add(1, std::memory_order_relaxed);
}

std::string Base64::encode(char *input, uint32 length)
{
    using namespace CryptoPP;
    std::string encoded;
    StringSource ss(input, length, true, new Base64Encoder(new StringSink(encoded), false));

    return encoded;
}

uint32 Base64::decode(char *input, uint32 length, char *out, uint32 out_length)
{
    using namespace CryptoPP;
    Base64Decoder decoder;
    decoder.Put((CryptoPP::byte*)input, length);
    decoder.MessageEnd();
    uint32 decoded_length = decoder.MaxRetrievable();

    if(decoded_length > out_length)
    {
        return decoded_length;
    }

    decoder.Get(out, decoded_length);

    return decoded_length;
}

std::string Sha1::hash(char *input, uint32 length)
{
    using namespace CryptoPP;
    SHA1 sha1;
    char digest[SHA1::DIGESTSIZE] = {0};
    sha1.CalculateDigest(digest, input, length);
    std::string output;    
    HexEncoder encoder;
    encoder.Attach(new StringSink(output));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    return output;
}

uint32 random_32()
{
    static std::random_device rd;
    static std::mt19937 mt(rd());

    return mt();
}

uint64 random_64()
{
    static std::random_device rd;
    static std::mt19937_64 mt(rd());
    
    return mt();
}

void crash_me()
{
    uint32 *p = NULL;
    *p = 1234567;
}

uint32 random_between(uint32 min, uint32 max)
{
    if(min == max)
    {
        return min;
    }

    if(min > max)
    {
        std::swap(min, max);
    }
    
    const uint32 diff = max - min + 1;
    
    return min + random_32() % diff;
}

bool rate_by_percent(uint32 rate)
{
    return random_between(1, 100) <= rate;
}

bool rate_by_thousand(uint32 rate)
{
    return random_between(1, 1000) <= rate;
}

}
}
