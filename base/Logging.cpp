/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <assert.h>
#include <thread>
#include <sstream>
#include "Logging.h"

void defultOutPut(const char *msg, int len) {
    size_t n = fwrite(msg, 1, len, stdout);
}

void defaultFlush() {
    fflush(stdout);
}

// __thread是GCC的一个线程局部存储设施，类似全局变量，但是各个线程一个独立的值，互不影响
__thread char t_errnobuf[512];
__thread char t_time[32];
__thread time_t t_lastSecond;

Logger::OutputFunc g_output = defultOutPut;
Logger::Flushfunc g_flush = defaultFlush;
TimeZone g_logTimeZone;

class T {
public:
    T(const char *str, unsigned len) : str_(str), len_(len) {
        assert(::strlen(str) == len);
    }

public:
    const char *str_;
    const unsigned len_;

};

inline LogStream &operator << (LogStream &s, T v) {
    s.append(v.str_, v.len_);
    return s;
}

Logger::Logger(SourceFile file, int line) : impl(INFO, 0, file, line) { }

Logger::Logger(SourceFile file, int line, LogLevel level) : impl(level, 0, file, line) { }

Logger::Logger(SourceFile file, int line, LogLevel level, const char *func) : impl(level, 0, file, line) {
    impl.stream << func << ' ';
}

Logger::Logger(SourceFile file, int line, bool toAbort) : impl(toAbort?FATAL:ERROR, errno, file, line) {}

Logger::~Logger() {
    impl.finish();
    const FixedBuffer<LogStream::kSmallBuffer> &buf(stream().buffer());
    g_output(buf.data(), buf.length());
    if (impl.level == FATAL) {
        g_flush();
        abort();
    }
}

void Logger::setLogLevel(LogLevel level) {
    g_logLevel = level;
}

void Logger::setOutput(OutputFunc out) {
    g_output = out;
}

void Logger::setFlush(Flushfunc flush) {
    g_flush = flush;
}

void Logger::setTimeZone(const TimeZone &tz) {
    g_logTimeZone = tz;
}

void Logger::Impl::formatTime() {
    int64_t microSecondsSinceEpoch = time.microSecondsSinceEpoch();
    time_t seconds = static_cast<time_t > (microSecondsSinceEpoch / TimeStamp::kMicroSecondsPerSecond);
    int microseconds = static_cast<int> (microSecondsSinceEpoch % TimeStamp::kMicroSecondsPerSecond);

    if (seconds != t_lastSecond) {
        t_lastSecond = seconds;
        struct tm tm_time;
        if (g_logTimeZone.valid()) {
            // 转成本地时间
            tm_time = g_logTimeZone.toLocalTime(seconds);
        } else {
            ::gmtime_r(&seconds, &tm_time);
        }

        int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d", tm_time.tm_year + 1970
                , tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        assert(len == 17);
    }

    if (g_logTimeZone.valid()) {
        Fmt us(".%06d ", microseconds);
        assert(us.length() == 8);
        stream << T(t_time, 17) << T(us.data(), 8);
    } else {
        Fmt us(".%06Z ", microseconds);
        assert(us.length() == 9);
        stream << T(t_time, 17) << T(us.data(), 9);
    }
}

static std::string getThreadIdStr() {
    std::string tmp;
    std::stringstream ss;
    ss << std::this_thread::get_id();
    ss >> tmp;
    return tmp;
}

const char *LogLevelName[Logger::NUM_LOG_LEVELS] = {"TRACE ", "DEBUG ", "INFO  ", "WARN  ", "ERROR ", "FATAL "};

Logger::Impl::Impl(Logger::LogLevel level, int savedErrno, const SourceFile &file, int line)
        : time(TimeStamp::now()), stream(), level(level), line(line), basename(file) {
    formatTime();
    std::string tmp = getThreadIdStr();
    stream << T(tmp.c_str(), tmp.size());
    stream << T(LogLevelName[level], 6);

    if (savedErrno != 0) {
        stream << strerror_tl(savedErrno) << " (errno=" << savedErrno << ")";
    }
}

inline LogStream &operator << (LogStream &s, const Logger::SourceFile &v) {
    s.append(v.data, v.size);
    return s;
}

void Logger::Impl::finish() {
    stream << " - " << basename << ":" << line << "\n";
}
