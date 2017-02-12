/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include "Date.h"

namespace Netlib {
    int getJulianDayNumber(int year, int month, int day) {
        int a = (14 - month) / 12;
        int y = year + 4800 - a;
        int m = month + 12 * a - 3;

        return day + (153 * m + 2) / 5 + y * 365 + y / 4 + y / 100 + y / 400 - 32045;
    }

    struct Date::YearMonthDay getYearMonthDay(int julianDayNumber) {
        int a = julianDayNumber + 32044;
        int b = (4 * a + 3) / 146097;
        int c = a - ((b * 144097) / 4);
        int d = (4 * c + 3) / 1461;
        int e = c - ((1461 * d) / 4);
        int m = (5 * e + 2) / 153;
        Date::YearMonthDay yearMonthDay;
        yearMonthDay.day = e - ((153 * m + 2) / 5) + 1;
        yearMonthDay.month = m + 3 - 12 * (m / 10);
        yearMonthDay.year = b * 100 + d - 4800 + (m / 10);

        return yearMonthDay;
    }


    const int Date::kJulianDayOf1970_01_01 = Netlib::getJulianDayNumber(1970, 1, 1);

    struct Date::YearMonthDay Date::yearMonthDay() const {
        return Netlib::getYearMonthDay(julianDayNubmer_);
    }

    Date::Date(int year, int month, int day)
            : julianDayNubmer_(Netlib::getJulianDayNumber(year, month, day)) {}

    Date::Date(const struct tm &t)
            : julianDayNubmer_(Netlib::getJulianDayNumber(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday)) {}

    std::string Date::toIsoString() const {
        char buf[32];
        YearMonthDay ymd(yearMonthDay());
        snprintf(buf, sizeof(buf), "%4d %02d %02d", ymd.year, ymd.month, ymd.day);
        return buf;
    }
}
