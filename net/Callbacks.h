/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_CALLBACKS_H
#define NETLIB_CALLBACKS_H

#include <functional>

namespace Netlib {
    typedef std::function<void()> TimerCallback;
}

#endif //NETLIB_CALLBACKS_H
