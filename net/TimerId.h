/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_TIMERID_H
#define NETLIB_TIMERID_H

#include <memory>

#include "Timer.h"

namespace Netlib {
    class TimerId{
    public:
        TimerId(std::unique_ptr<Timer> &&value) : value_(std::move(value)) {}
        TimerId(std::unique_ptr<Timer> &value) : value_(std::move(value)) {}
        TimerId(Timer *value) {
            value_.reset(value);
        }

    private:
        std::unique_ptr<Timer> value_;
    };
}

#endif //NETLIB_TIMERID_H
