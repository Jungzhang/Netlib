/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_DATE_H
#define NETLIB_DATE_H

#include <algorithm>

namespace Netlib {

    class Date {

    public:

        static const int kDaysPerWeek = 7;
        static const int kJulianDayOf1970_01_01;

        struct YearMonthDay {
            int year;
            int month;
            int day;
        };

    public:
        Date() : julianDayNubmer_(0) {}

        Date(int year, int month, int day);

        explicit Date(int julianDayNum) : julianDayNubmer_(julianDayNum) {}

        explicit Date(const struct tm &t);

        void swap(Date &that) {
            std::swap(julianDayNubmer_, that.julianDayNubmer_);
        }

        bool valid() const {
            return julianDayNubmer_ > 0;
        }

        std::string toIsoString() const;

        struct YearMonthDay yearMonthDay() const;

        int year() const {
            return yearMonthDay().year;
        }

        int month() const {
            return yearMonthDay().month;
        }

        int day() const {
            return yearMonthDay().day;
        }

        int weekDay() const {
            return (julianDayNubmer_ + 1) % kDaysPerWeek;
        }

        int julianDayNumber() const {
            return julianDayNubmer_;
        }

    private:
        int julianDayNubmer_;
    };

    inline bool operator<(Date x, Date y) {
        return x.julianDayNumber() < y.julianDayNumber();
    }

    inline bool operator==(Date x, Date y) {
        return x.julianDayNumber() == y.julianDayNumber();
    }
}


#endif //NETLIB_DATE_H
