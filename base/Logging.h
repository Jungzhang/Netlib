/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_LOGGING_H
#define NETLIB_LOGGING_H


#include <cstring>
#include "TimeStamp.h"
#include "LogStream.h"
#include "TimeZone.h"

class Logger {
public:
    enum LogLevel {
        TRACE,      // 跟踪
        DEBUG,      // debug
        INFO,       // 信息
        WANR,       // 警告
        ERROR,      // 错误
        FATAL,      // 致命性的
        NUM_LOG_LEVELS
    };

    class SourceFile {
    public:
        template <int N>

        // 构造函数
        inline SourceFile(const char (&arr)[N]) : data(arr), size(N - 1) {
            const char *slash = ::strrchr(data, '/');   // 查找末次出现/的位置
            if (slash) {
                data = slash + 1;
                size -= static_cast<int> (data - arr);
            }
        }

        // 构造函数(显式)
        explicit SourceFile(const char *filename) : data(filename) {
            const char *slash = ::strrchr(filename, '/');
            if (slash) {
                data = slash + 1;
            }
            size = static_cast<int>(::strlen(data));
        }

    public:
        const char *data;
        int size;
    };

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char *func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream &stream() {
        return impl.stream;
    }

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);
    typedef void (*OutputFunc)(const char *msg, int len);
    typedef void (*Flushfunc)();
    static void setOutput(OutputFunc);
    static void setFlush(Flushfunc);
    static void setTimeZone(const TimeZone &tz);

private:
    class Impl {
    public:
        Impl(LogLevel level, int old_error, const SourceFile &file, int line);
        void formatTime();
        void finish();

    public:
        TimeStamp time;
        LogStream stream;
        LogLevel level;
        int line;
        SourceFile basename;
    };

    Impl impl;

};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel() {

}


#endif //NETLIB_LOGGING_H
