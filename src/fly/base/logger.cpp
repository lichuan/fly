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
 *   @date: 2015-06-10 18:25:08                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <cstdarg>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include "fly/base/logger.hpp"

namespace fly {
namespace base {

bool Logger::init(LOG_LEVEL level, const std::string &app, const std::string &path)
{
    m_level = level;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm tm;
    localtime_r(&tv.tv_sec, &tm);
    m_year.store(1900 + tm.tm_year, std::memory_order_relaxed);
    m_month.store(1 + tm.tm_mon, std::memory_order_relaxed); 
    m_day.store(tm.tm_mday, std::memory_order_relaxed);
    
    if(mkpath(path) == -1)
    {
        CONSOLE_ONLY("mkdir path: %s failed, reason: %s", path.c_str(), strerror(errno));
        return false;
    }
    
    if(path[path.length() - 1] != '/')
    {
        const_cast<std::string&>(path).append("/");
    }
    
    m_file_name = path + app;
    m_file_full_name = path + app + ".log";
    m_file = fopen(m_file_full_name.c_str(), "rb");
    
    if(m_file != NULL)
    {
        int32 fd = fileno(m_file);
        struct stat st;
        fstat(fd, &st);
        struct tm tm_1;
        localtime_r(&st.st_mtim.tv_sec, &tm_1);
        uint32 year = 1900 + tm_1.tm_year;
        uint32 month = 1 + tm_1.tm_mon;
        uint32 day = tm_1.tm_mday;
        
        if(year != m_year.load(std::memory_order_relaxed) || month != m_month.load(std::memory_order_relaxed) || day != m_day.load(std::memory_order_relaxed))
        {
            char file_name[64];
            sprintf(file_name, "%s_%d-%02d-%02d.log", m_file_name.c_str(), year, month, day);
            fclose(m_file);
            std::rename(m_file_full_name.c_str(), file_name);
        }
        else
        {
            fclose(m_file);
        }
    }
    
    m_file = fopen(m_file_full_name.c_str(), "ab");
    setvbuf(m_file, NULL, _IOLBF, 1024);
    return true;
}

void Logger::_log(uint32 year, uint32 month, uint32 day, const char *format, ...)
{    
    if(year != m_year.load(std::memory_order_relaxed) || month != m_month.load(std::memory_order_relaxed)
       || day != m_day.load(std::memory_order_relaxed))
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        uint32 y = m_year.load(std::memory_order_relaxed);
        uint32 m = m_month.load(std::memory_order_relaxed);
        uint32 d = m_day.load(std::memory_order_relaxed);
        
        //double-checked
        if(year != y || month != m || day != d)
        {
            char file_name[64];
            sprintf(file_name, "%s_%d-%02d-%02d.log", m_file_name.c_str(), y, m, d);
            fclose(m_file);
            std::rename(m_file_full_name.c_str(), file_name);
            m_file = fopen(m_file_full_name.c_str(), "ab");
            setvbuf(m_file, NULL, _IOLBF, 1024);
            m_year.store(year, std::memory_order_relaxed);
            m_month.store(month, std::memory_order_relaxed);
            m_day.store(day, std::memory_order_relaxed);
        }
        
        va_list args;
        va_start(args, format);
        vfprintf(m_file, format, args);
        va_end(args);
    }
    else
    {
        va_list args;
        va_start(args, format);
        vfprintf(m_file, format, args);
        va_end(args);
    }
}

void Logger::_console_log(uint32 year, uint32 month, uint32 day, const char *format, ...)
{    
    if(year != m_year.load(std::memory_order_relaxed) || month != m_month.load(std::memory_order_relaxed)
       || day != m_day.load(std::memory_order_relaxed))
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        uint32 y = m_year.load(std::memory_order_relaxed);
        uint32 m = m_month.load(std::memory_order_relaxed);
        uint32 d = m_day.load(std::memory_order_relaxed);
        
        //double-checked
        if(year != y || month != m || day != d)
        {
            char file_name[64];
            sprintf(file_name, "%s_%d-%02d-%02d.log", m_file_name.c_str(), y, m, d);
            fclose(m_file);
            std::rename(m_file_full_name.c_str(), file_name);
            m_file = fopen(m_file_full_name.c_str(), "ab");
            setvbuf(m_file, NULL, _IOLBF, 1024);
            m_year.store(year, std::memory_order_relaxed);
            m_month.store(month, std::memory_order_relaxed);
            m_day.store(day, std::memory_order_relaxed);
        }
        
        va_list args;
        va_start(args, format);
        vfprintf(m_file, format, args);
        va_end(args);
    }
    else
    {
        va_list args;
        va_start(args, format);
        vfprintf(m_file, format, args);
        va_end(args);
    }

    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

void Logger::_console_only(uint32 year, uint32 month, uint32 day, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

}
}
