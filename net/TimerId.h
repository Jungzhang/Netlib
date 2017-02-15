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
        explicit TimerId(Timer *value) : value_(value) { }

    private:
        // FIXME 不确定对不对
        Timer* value_;
    };
}

#endif //NETLIB_TIMERID_H
