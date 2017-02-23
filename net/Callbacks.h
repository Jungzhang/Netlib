/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_CALLBACKS_H
#define NETLIB_CALLBACKS_H

#include <functional>
#include <memory>
#include "Buffer.h"
#include "../base/TimeStamp.h"

namespace Netlib {

    class TcpConnection;

    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

    typedef std::function<void()> TimerCallback;
    typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;
    typedef std::function<void(const TcpConnectionPtr &, Buffer *buf, TimeStamp)> MessageCallback;
    typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;
    typedef std::function<void(const TcpConnectionPtr &)> WriteCompleteCallback;
}

#endif //NETLIB_CALLBACKS_H
