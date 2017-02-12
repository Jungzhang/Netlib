/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_TIMEZONE_H
#define NETLIB_TIMEZONE_H


#include <memory>

namespace Netlib {

    class TimeZone {
    public:

        // 构造函数
        explicit TimeZone(const char *zonefile);

        TimeZone(int eastOfUtc, const char *name);  // 固定时区
        TimeZone() {}   // 非法时区

        // 是否有效
        bool valid() const {
            return static_cast<bool> (data_);
        }

        // localTime相关
        struct tm toLocalTime(time_t secondsSinceEpoch) const;

        time_t formLocalTime(const struct tm &t) const;

        // UTCTime相关
        static struct tm toUtcTime(time_t secondsSinceEpoch, bool yday = false);

        static time_t formUtcTime(const struct tm &t);

        static time_t formUtcTime(int year, int month, int day, int hour, int min, int second);

    public:
        struct Data; // 数据类

    private:
        std::shared_ptr<Data> data_;  // 数据类指针,采用shared_ptr管理内存

    };
}


#endif //NETLIB_TIMEZONE_H
