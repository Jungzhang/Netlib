/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_TIMERID_H
#define NETLIB_TIMERID_H

#include <memory>

namespace Netlib {
    class Timer;
    class TimerId{
    public:
        explicit TimerId(Timer *value = nullptr, int64_t seq = 0) : value_(value), sequence_(seq) { }
        friend class TimerQueue;
    private:
        Timer* value_;
        int64_t sequence_;
    };
}

#endif //NETLIB_TIMERID_H
