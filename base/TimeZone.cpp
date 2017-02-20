/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <vector>
#include <cassert>
#include <algorithm>
#include <string.h>

#include "Date.h"
#include "TimeZone.h"

namespace Netlib {

    struct Transition {
        time_t gmttime;
        time_t localtime;
        int localtimeIdx;

        Transition(time_t t, time_t l, int localIdx)
                : gmttime(t), localtime(l), localtimeIdx(localIdx) {}
    };

    struct Comp {
        bool compareGmt;

        Comp(bool gmt) : compareGmt(gmt) {}

        bool operator()(const Transition &lhs, const Transition &rhs) const {
            if (compareGmt) {
                return lhs.gmttime < lhs.gmttime;
            } else {
                return lhs.localtime < rhs.localtime;
            }
        }

        bool equal(const Transition &lhs, const Transition &rhs) const {
            if (compareGmt) {
                return lhs.gmttime == lhs.gmttime;
            } else {
                return lhs.localtime == lhs.localtime;
            }
        }
    };

    struct LocalTime {
        time_t gmtOffset;
        bool isDst;
        int arrbIdx;

        LocalTime(time_t offset, bool dst, int abbr)
                : gmtOffset(offset), isDst(dst), arrbIdx(abbr) {}
    };

    class File {
    public:
        // 构造函数
        File(const char *file) : fp_(::fopen(file, "rb")) {}

        // 析构函数
        ~File() {
            if (fp_) {
                ::fclose(fp_);
            }
        }

        // 打开的文件是否有效
        bool valid() const {
            return fp_ != nullptr;
        }

        // 读取n字节
        std::string readBytes(int n) {
            assert(n > 0);
            char buf[n];
            ssize_t nr = ::fread(buf, 1, n, fp_);
            if (nr != n) {
                throw std::logic_error("no enough data");
            }
            return std::string(buf, n);
        }

        // 读取相关数据
        int32_t readInt32() {
            int32_t x = 0;
            ssize_t nr = ::fread(&x, 1, sizeof(int32_t), fp_);
            if (nr != sizeof(int32_t)) {
                throw std::logic_error("no enough data");
            }
            return be32toh(x);  // 大端转主机32位
        }

        uint8_t readUInt8() {
            uint8_t x = 0;
            ssize_t nr = ::fread(&x, 1, sizeof(uint8_t), fp_);
            if (nr != sizeof(uint8_t)) {
                throw std::logic_error("no enough data");
            }
            return x;
        }

    private:
        FILE *fp_;
    };

    const int kSecondsPerDay = 24 * 60 * 60;

    struct TimeZone::Data {
        std::string abbreviation;  // 缩写
        std::vector<std::string> names;
        std::vector<Netlib::Transition> transitions;
        std::vector<Netlib::LocalTime> localTimes;
    };

    // 读取并解析时区信息文件
    bool readTimeZoneFile(const char *zonefile, struct TimeZone::Data *data) {
        File f(zonefile);
        if (f.valid()) {
            try {
                std::string head = f.readBytes(4);
                if (head != "TZif") {
                    throw std::logic_error("bad head");
                }
                std::string version = f.readBytes(1);
                f.readBytes(15);

                int32_t isgmtcnt = f.readInt32();
                int32_t isstdcnt = f.readInt32();
                int32_t leapcnt = f.readInt32();
                int32_t timecnt = f.readInt32();
                int32_t typecnt = f.readInt32();
                int32_t charcnt = f.readInt32();

                std::vector<int32_t> trans;
                std::vector<int> localtimes;
                trans.reserve(timecnt);  // 预分配的大小

                for (int i = 0; i < timecnt; ++i) {
                    trans.push_back(f.readInt32());
                }

                for (int i = 0; i < timecnt; ++i) {
                    uint8_t local = f.readUInt8();
                    localtimes.push_back(local);
                }

                for (int i = 0; i < typecnt; ++i) {
                    int32_t gmtoff = f.readInt32();
                    uint8_t isdst = f.readUInt8();
                    uint8_t abbrind = f.readUInt8();
                    data->localTimes.push_back(LocalTime(gmtoff, isdst, abbrind));
                }

                for (int i = 0; i < timecnt; ++i) {
                    int localIdx = localtimes[i];
                    time_t locatime = trans[i] + data->localTimes[localIdx].gmtOffset;
                    data->transitions.push_back(Transition(trans[i], locatime, localIdx));
                }

                data->abbreviation = f.readBytes(charcnt);

//                for (int i = 0; i < leapcnt; ++i) {
//
//                }
//                (void) isstdcnt;
//                (void) isgmtcnt;
            } catch (std::logic_error &e) {
                fprintf(stderr, "%s\n", e.what());
            }
        }

        return true;
    }

    inline void fillHMS(unsigned seconds, struct tm *utc) {
        utc->tm_sec = seconds % 60;
        unsigned minutes = seconds / 60;
        utc->tm_min = minutes % 60;
        utc->tm_hour = minutes / 60;
    }

    const LocalTime *findLocalTime(const TimeZone::Data &data, Transition sentry, Comp comp) {

        const LocalTime *local = nullptr;
        if (data.transitions.empty() || comp(sentry, data.transitions.front())) {
            local = &data.localTimes.front();
        } else {
            std::vector<Transition>::const_iterator transI =
                    std::lower_bound(data.transitions.begin(), data.transitions.end(), sentry, comp);

            if (transI != data.transitions.end()) {
                if (!comp.equal(sentry, *transI)) {
                    assert(transI != data.transitions.begin());
                    --transI;
                }
                local = &data.localTimes[transI->localtimeIdx];
            } else {
                local = &data.localTimes[data.transitions.back().localtimeIdx];
            }
        }
        return local;
    }


    TimeZone::TimeZone(const char *zonefile) : data_(new TimeZone::Data) {
        if (!Netlib::readTimeZoneFile(zonefile, data_.get())) {
            data_.reset();
        }
    }

    TimeZone::TimeZone(int eastOfUtc, const char *name) : data_(new TimeZone::Data) {
        data_->localTimes.push_back(Netlib::LocalTime(eastOfUtc, false, 0));
        data_->abbreviation = name;
    }

    struct tm TimeZone::toLocalTime(time_t secondsSinceEpoch) const {
        struct tm localtime;
        ::bzero(&localtime, sizeof(localtime));
        assert(data_ != nullptr);
        const Data &data(*data_);

        Netlib::Transition sentry(secondsSinceEpoch, 0, 0);
        const Netlib::LocalTime *local = Netlib::findLocalTime(data, sentry, Netlib::Comp(true));

        if (local) {
            time_t locaSeconds = secondsSinceEpoch + local->gmtOffset;
            ::gmtime_r(&locaSeconds, &localtime);
            localtime.tm_isdst = local->isDst;
            localtime.tm_gmtoff = local->gmtOffset;
            localtime.tm_zone = &data.abbreviation[local->arrbIdx];
        }

        return localtime;
    }

    time_t TimeZone::formLocalTime(const struct tm &t) const {
        assert(data_ != nullptr);
        const Data &data(*data_);

        struct tm tmp = t;
        time_t second = ::timegm(&tmp);
        Netlib::Transition sentry(0, second, 0);
        const Netlib::LocalTime *local = Netlib::findLocalTime(data, sentry, Netlib::Comp(false));
        if (t.tm_isdst) {
            struct tm tryTm = toLocalTime(second - local->gmtOffset);
            if (!tryTm.tm_isdst
                && tryTm.tm_hour == t.tm_hour
                && tryTm.tm_min == t.tm_min) {
                second -= 3600;
            }
        }
        return second - local->gmtOffset;
    }

    struct tm TimeZone::toUtcTime(time_t secondsSinceEpoch, bool yday) {
        struct tm utc;
        bzero(&utc, sizeof(utc));
        utc.tm_zone = "GMT";
        int seconds = static_cast<int>(secondsSinceEpoch % Netlib::kSecondsPerDay);
        int days = static_cast<int>(secondsSinceEpoch / Netlib::kSecondsPerDay);
        if (seconds < 0) {
            seconds += Netlib::kSecondsPerDay;
            --days;
        }
        Netlib::fillHMS(seconds, &utc);
        Date date(days + Date::kJulianDayOf1970_01_01);
        Date::YearMonthDay ymd = date.yearMonthDay();
        utc.tm_year = ymd.year - 1900;
        utc.tm_mon = ymd.month - 1;
        utc.tm_mday = ymd.day;
        utc.tm_wday = date.weekDay();

        if (yday) {
            Date startOfYear(ymd.year, 1, 1);
            utc.tm_yday = date.julianDayNumber() - startOfYear.julianDayNumber();
        }
        return utc;
    }

    time_t TimeZone::formUtcTime(int year, int month, int day,
                                 int hour, int minute, int seconds) {
        Date date(year, month, day);
        int secondsInDay = hour * 3600 + minute * 60 + seconds;
        time_t days = date.julianDayNumber() - Date::kJulianDayOf1970_01_01;
        return days * Netlib::kSecondsPerDay + secondsInDay;
    }

    time_t TimeZone::formUtcTime(const struct tm &utc) {
        return formUtcTime(utc.tm_year + 1900, utc.tm_mon + 1, utc.tm_mday,
                           utc.tm_hour, utc.tm_min, utc.tm_sec);
    }
}