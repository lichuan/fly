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
 *   @date: 2015-06-10 18:25:08                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <cstdarg>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "fly/base/logger.hpp"

namespace fly {
namespace base {

void Logger::init(fly::base::LEVEL level, const std::string &app, const std::string &path)
{
    m_level = level;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm tm;
    localtime_r(&tv.tv_sec, &tm);
    m_year = 1900 + tm.tm_year;
    m_month = 1 + tm.tm_mon;
    m_day = tm.tm_mday;
    m_file_name = path + app;
    m_file_full_name = path + app + ".log";
    m_file = fopen(m_file_full_name.c_str(), "r");
    
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
        
        if(year != m_year || month != m_month || day != m_day)
        {
            char file_name[64];
            sprintf(file_name, "%s_%d-%02d-%02d.log", m_file_name.c_str(), year, month, day);
            fclose(m_file);
            std::rename(m_file_full_name.c_str(), file_name);
        }
    }

    m_file = fopen(m_file_full_name.c_str(), "ab");
}

void Logger::log_impl(fly::base::LEVEL level, uint32 year, uint32 month, uint32 day, const char *format, ...)
{
    if(level < m_level)
    {
        return;
    }
    
    if(m_year != year || m_month != month || m_day != day)
    {
        std::lock_guard<std::mutex> guard(m_mutex);

        if(m_year != year || m_month != month || m_day != day) //double checked
        {
            char file_name[64];
            sprintf(file_name, "%s_%d-%02d-%02d.log", m_file_name.c_str(), m_year, m_month, m_day);
            fclose(m_file);
            std::rename(m_file_full_name.c_str(), file_name);
            m_file = fopen(m_file_full_name.c_str(), "ab");
            m_year = year;
            m_month = month;
            m_day = day;
        }
    }
    
    va_list args;
    va_start(args, format);
    vfprintf(m_file, format, args);
    va_end(args);
    fflush(m_file);
}

}
}
