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
 *   @date: 2015-06-10 17:27:54                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FLY__BASE__LOGGER
#define FLY__BASE__LOGGER

#include <cstdio>
#include <string>
#include <mutex>
#include <sys/time.h>
#include "fly/base/common.hpp"
#include "fly/base/singleton.hpp"

namespace fly {
namespace base {

enum LEVEL
{
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    MAX_LEVEL
};

class Logger : public Singleton<Logger>
{
public:
    void init(LEVEL level, const std::string &app, const std::string &path);
    void log_impl(LEVEL level, uint32 year, uint32 month, uint32 day, const char *format, ...);
    
private:
    LEVEL m_level;
    std::string m_file_name;
    std::string m_file_full_name;
    FILE *m_file;
    volatile uint32 m_year;
    volatile uint32 m_month;
    volatile uint32 m_day;
    std::mutex m_mutex;
};

}
}

#define LOG_DEBUG(format, ...) \
    char _format_[2048]; \
    struct timeval _tv_; \
    gettimeofday(&_tv_, NULL); \
    struct tm _tm_; \
    localtime_r(&_tv_.tv_sec, &_tm_); \
    sprintf(_format_, "%d-%02d-%02d %02d:%02d:%02d.%06ld |debug| %s:%d {%s}\n", 1900 + _tm_.tm_year, 1 + _tm_.tm_mon, _tm_.tm_mday, _tm_.tm_hour, \
            _tm_.tm_min, _tm_.tm_sec, _tv_.tv_usec, __FILE__, __LINE__, format); \
    fly::base::Logger::instance()->log_impl(fly::base::DEBUG, 1900 + _tm_.tm_year, 1 + _tm_.tm_mon, _tm_.tm_mday, _format_, ##__VA_ARGS__)

#define LOG_INFO(format, ...) \
    char _format_[2048]; \
    struct timeval _tv_; \
    gettimeofday(&_tv_, NULL); \
    struct tm _tm_; \
    localtime_r(&_tv_.tv_sec, &_tm_); \
    sprintf(_format_, "%d-%02d-%02d %02d:%02d:%02d.%06ld |info| %s:%d {%s}\n", 1900 + _tm_.tm_year, 1 + _tm_.tm_mon, _tm_.tm_mday, _tm_.tm_hour, \
            _tm_.tm_min, _tm_.tm_sec, _tv_.tv_usec, __FILE__, __LINE__, format); \
    fly::base::Logger::instance()->log_impl(fly::base::INFO, 1900 + _tm_.tm_year, 1 + _tm_.tm_mon, _tm_.tm_mday, _format_, ##__VA_ARGS__)

#define LOG_WARN(format, ...) \
    char _format_[2048]; \
    struct timeval _tv_; \
    gettimeofday(&_tv_, NULL); \
    struct tm _tm_; \
    localtime_r(&_tv_.tv_sec, &_tm_); \
    sprintf(_format_, "%d-%02d-%02d %02d:%02d:%02d.%06ld |warn| %s:%d {%s}\n", 1900 + _tm_.tm_year, 1 + _tm_.tm_mon, _tm_.tm_mday, _tm_.tm_hour, \
            _tm_.tm_min, _tm_.tm_sec, _tv_.tv_usec, __FILE__, __LINE__, format); \
    fly::base::Logger::instance()->log_impl(fly::base::WARN, 1900 + _tm_.tm_year, 1 + _tm_.tm_mon, _tm_.tm_mday, _format_, ##__VA_ARGS__)

#define LOG_ERROR(format, ...) \
    char _format_[2048]; \
    struct timeval _tv_; \
    gettimeofday(&_tv_, NULL); \
    struct tm _tm_; \
    localtime_r(&_tv_.tv_sec, &_tm_); \
    sprintf(_format_, "%d-%02d-%02d %02d:%02d:%02d.%06ld |error| %s:%d {%s}\n", 1900 + _tm_.tm_year, 1 + _tm_.tm_mon, _tm_.tm_mday, _tm_.tm_hour, \
            _tm_.tm_min, _tm_.tm_sec, _tv_.tv_usec, __FILE__, __LINE__, format); \
    fly::base::Logger::instance()->log_impl(fly::base::ERROR, 1900 + _tm_.tm_year, 1 + _tm_.tm_mon, _tm_.tm_mday, _format_, ##__VA_ARGS__)

#define LOG_FATAL(format, ...) \
    char _format_[2048]; \
    struct timeval _tv_; \
    gettimeofday(&_tv_, NULL); \
    struct tm _tm_; \
    localtime_r(&_tv_.tv_sec, &_tm_); \
    sprintf(_format_, "%d-%02d-%02d %02d:%02d:%02d.%06ld |fatal| %s:%d {%s}\n", 1900 + _tm_.tm_year, 1 + _tm_.tm_mon, _tm_.tm_mday, _tm_.tm_hour, \
            _tm_.tm_min, _tm_.tm_sec, _tv_.tv_usec, __FILE__, __LINE__, format); \
    fly::base::Logger::instance()->log_impl(fly::base::FATAL, 1900 + _tm_.tm_year, 1 + _tm_.tm_mon, _tm_.tm_mday, _format_, ##__VA_ARGS__)

#endif
