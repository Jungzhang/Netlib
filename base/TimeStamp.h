/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_TIMESTAMP_H
#define NETLIB_TIMESTAMP_H

#include <zconf.h>
#include <algorithm>

class TimeStamp {

public:

    //构造函数
    TimeStamp() : microSecondsSinceEpoch_(0) {

    }

    explicit TimeStamp(int64_t microSecondsSinceEpochArg)
            : microSecondsSinceEpoch_(microSecondsSinceEpochArg) {

    }

    // 交换
    void swap(TimeStamp &that) {
        std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
    }

    std::string toString() const ;
    std::string toFormattedString(bool showMicroSeconds = true) const ;

    // 时间有效性
    bool vaild() const {
        return microSecondsSinceEpoch_ > 0;
    }

    // 返回原始时间
    int64_t microSecondsSinceEpoch() const {
        return microSecondsSinceEpoch_;
    }

    // 返回time_t的时间
    time_t secondsSinceEpoch() const {
        return static_cast<time_t >(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
    }

    // 获取现在的时间
    static TimeStamp now();

    // 返回一个无效时间
    static TimeStamp invalid() {
        return TimeStamp();
    }

    // time_t转TimeStamp
    static TimeStamp formUnixTime(time_t t) {
        return formUnixTime(t, 0);
    }

    static TimeStamp formUnixTime(time_t t, int microseconds) {
        return TimeStamp(static_cast<int64_t >(t) * kMicroSecondsPerSecond + microseconds);
    }

public:
    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t  microSecondsSinceEpoch_;    // 从UNIX纪元到现在的微秒数
};

// 重载一些运算符
inline bool operator < (TimeStamp lhs, TimeStamp rhs) {
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator == (TimeStamp lhs, TimeStamp rhs) {
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

// 计算两个时间的间隔(返回秒数)
inline double timeDifference(TimeStamp high, TimeStamp low) {
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double >(diff) / TimeStamp::kMicroSecondsPerSecond;
}

// 加上seconds秒
inline TimeStamp addTime(TimeStamp timeStamp, double second) {
    int64_t delta = static_cast<int64_t >(second * TimeStamp::kMicroSecondsPerSecond);
    return TimeStamp(delta + timeStamp.microSecondsSinceEpoch());
}

#endif //NETLIB_TIMESTAMP_H
