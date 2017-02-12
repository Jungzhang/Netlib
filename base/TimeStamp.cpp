/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <cinttypes>
#include <sys/time.h>
#include "TimeStamp.h"

//#ifndef __STDC_FORMAT_MACROS
//#define __STDC_FORMAT_MACROS
//#endif
namespace Netlib {

    TimeStamp TimeStamp::now() {
        struct timeval tv;
        ::gettimeofday(&tv, nullptr);
        int64_t seconeds = tv.tv_sec;
        return TimeStamp(seconeds * TimeStamp::kMicroSecondsPerSecond + tv.tv_usec);
    }

    std::string TimeStamp::toString() const {
        char buf[32] = {0, 0};
        int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
        int64_t microSeconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
        ::snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microSeconds);
        return buf;
    }

    std::string TimeStamp::toFormattedString(bool showMicroSeconds) const {
        char buf[32] = {0};
        time_t seconds = static_cast<time_t > (microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
        struct tm tm_time;
        ::gmtime_r(&seconds, &tm_time);   // 将时间转化为UTC时间,并存在用户提供的结构体中

        if (showMicroSeconds) {
            int microSencods = static_cast<int> (microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
            snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d", tm_time.tm_year + 1900,
                     tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                     microSencods);
        } else {
            snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d", tm_time.tm_year + 1900,
                     tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        }

        return buf;
    }
}
