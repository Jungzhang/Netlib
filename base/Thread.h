/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_THREAD_H
#define NETLIB_THREAD_H

#include <thread>

namespace Netlib{
    class Thread {
    public:
        static  pthread_t convertIdToInt(const std::thread::id &id);
    };
}

#endif //NETLIB_THREAD_H
