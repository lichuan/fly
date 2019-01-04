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
#include <netinet/in.h>
#include <sys/stat.h>
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
    //time too long to arrive at this condition, so, comment out this 'if' branch
    // if(m_id.load(std::memory_order_relaxed) == 18446744073709551615UL)
    // {
    //     m_id.store(1, std::memory_order_relaxed);
    // }
    
    return m_id.fetch_add(1, std::memory_order_relaxed);
}

std::string byte2hexstr(const char *input, uint32 length)
{
    using namespace CryptoPP;
    std::string encoded;
    StringSource ss(input, length, true, new HexEncoder(new StringSink(encoded), false));

    return encoded;
}

uint32 hexstr2byte(const char *input, uint32 length, char *out, uint32 out_length)
{
    using namespace CryptoPP;
    HexDecoder decoder;
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

std::string base64_encode(const char *input, uint32 length)
{
    using namespace CryptoPP;
    std::string encoded;
    StringSource ss(input, length, true, new Base64Encoder(new StringSink(encoded), false));

    return encoded;
}

uint32 base64_decode(const char *input, uint32 length, char *out, uint32 out_length)
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

bool sha1(const char *input, uint32 length, char *out, uint32 out_length)
{
    using namespace CryptoPP;
    SHA1 sha1;

    if(out_length < SHA1::DIGESTSIZE)
    {
        return false;
    }

    sha1.CalculateDigest(out, input, length);

    return true;
}

bool sha256(const char *input, uint32 length, char *out, uint32 out_length)
{
    using namespace CryptoPP;
    SHA256 sha256;

    if(out_length < SHA256::DIGESTSIZE)
    {
        return false;
    }

    sha256.CalculateDigest(out, input, length);

    return true;
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

void split_string(const std::string &str, const char *split, std::vector<std::string> &vec, char **save_ptr)
{
    if(str.empty())
    {
        return;
    }

    std::string str_copy = str;
    char *buf = str_copy.data();
    char *token = strtok_r(buf, split, save_ptr);
    
    while(token != NULL)
    {
        vec.push_back(token);
        token = strtok_r(NULL, split, save_ptr);
    }
}

int32 mkpath(std::string s, mode_t mode)
{
    size_t pre = 0, pos;
    std::string dir;
    int32 mdret;

    if(s[s.size() - 1] != '/')
    {
        s += '/';
    }
    
    while((pos = s.find_first_of('/', pre)) != std::string::npos)
    {
        dir = s.substr(0, pos++);
        pre = pos;
        
        if(dir.size() == 0)
        {
            continue;
        }
        
        if((mdret = ::mkdir(dir.c_str(), mode)) && errno != EEXIST)
        {
            return mdret;
        }
    }
    
    return 0;
}

uint64 htonll(uint64 n)
{
    return (((uint64)htonl((uint32)n)) << 32) | htonl(n >> 32);
}

uint64 ntohll(uint64 n)
{
    return (((uint64)ntohl((uint32)n)) << 32) | ntohl(n >> 32);
}

}
}
