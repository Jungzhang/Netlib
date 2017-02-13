/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <sstream>
#include "Thread.h"

namespace Netlib {
    pthread_t Thread::convertIdToInt(const std::thread::id &id) {
        pthread_t tmp;
        std::stringstream ss;
        ss << id;
        ss >> tmp;
        return tmp;
    }
}