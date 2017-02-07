/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Created Time: 17-2-7
* >Description: 
**********************************************/

#include "LogStream.h"

template <int SIZE>
void FixedBuffer<SIZE>::cookiesEnd() {

}

template <int SIZE>
void FixedBuffer<SIZE>::cookiesStart() {

}

template <int SIZE>
const char* FixedBuffer<SIZE>::debugString() {
    *cur = '\0';
    return data_;
}