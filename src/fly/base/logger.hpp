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
 *   @date: 2015-06-10 17:27:54                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__BASE__LOGGER
#define FLY__BASE__LOGGER

#include <cstdio>
#include <atomic>
#include <string>
#include <mutex>
#include <sys/time.h>
#include "fly/base/common.hpp"
#include "fly/base/singleton.hpp"

namespace fly {
namespace base {

enum LOG_LEVEL
{
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

class Logger : public Singleton<Logger>
{
public:
    bool init(LOG_LEVEL level, const std::string &app, const std::string &path);
    void _log(uint32 year, uint32 month, uint32 day, const char *format, ...);
    void _console_log(uint32 year, uint32 month, uint32 day, const char *format, ...);
    void _console_only(uint32 year, uint32 month, uint32 day, const char *format, ...);
    
    bool _enter(LOG_LEVEL level)
    {
        if(level < m_level)
        {
            return false;
        }

        return true;
    }
    
private:
    LOG_LEVEL m_level;
    std::string m_file_name;
    std::string m_file_full_name;
    FILE *m_file;
    std::atomic<uint32> m_year;
    std::atomic<uint32> m_month;
    std::atomic<uint32> m_day;
    std::mutex m_mutex;
};

}
}

#define LOG_DEBUG_INFO(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::DEBUG)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [info] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#define LOG_DEBUG_WARN(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::DEBUG)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [warn] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#define LOG_DEBUG_ERROR(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::DEBUG)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [error] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#define LOG_DEBUG_FATAL(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::DEBUG)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [fatal] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#define LOG_INFO(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::INFO)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [info] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#define LOG_WARN(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::WARN)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [warn] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#define LOG_ERROR(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::ERROR)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [error] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#define LOG_FATAL(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::FATAL)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [fatal] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

// the following macro also log to console
#define CONSOLE_LOG_DEBUG_INFO(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::DEBUG)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [info] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_console_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#define CONSOLE_LOG_DEBUG_WARN(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::DEBUG)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [warn] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_console_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#define CONSOLE_LOG_DEBUG_ERROR(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::DEBUG)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [error] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_console_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#define CONSOLE_LOG_DEBUG_FATAL(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::DEBUG)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [fatal] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_console_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#define CONSOLE_LOG_INFO(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::INFO)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [info] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_console_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#define CONSOLE_LOG_WARN(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::WARN)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [warn] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_console_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#define CONSOLE_LOG_ERROR(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::ERROR)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [error] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_console_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#define CONSOLE_LOG_FATAL(format, ...) \
    if(fly::base::Logger::instance()->_enter(fly::base::FATAL)) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [fatal] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_console_log(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#define CONSOLE_ONLY(format, ...) \
    { \
        char _format[256]; \
        struct timeval _tv; \
        gettimeofday(&_tv, NULL); \
        struct tm _tm; \
        localtime_r(&_tv.tv_sec, &_tm); \
        sprintf(_format, "%d-%02d-%02d %02d:%02d:%02d.%06ld [utc:%lu] [console] %s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
                _tm.tm_min, _tm.tm_sec, _tv.tv_usec, _tv.tv_sec, __FILE__, __LINE__, format); \
        fly::base::Logger::instance()->_console_only(1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _format, ##__VA_ARGS__); \
    }

#endif
