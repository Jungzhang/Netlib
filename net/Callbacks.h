/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_CALLBACKS_H
#define NETLIB_CALLBACKS_H

#include <functional>
#include <memory>

namespace Netlib {

    class TcpConnection;

    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

    typedef std::function<void()> TimerCallback;
    typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;
    typedef std::function<void(const TcpConnectionPtr &, const char *data, ssize_t len)> MessageCallback;
    typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;
}

#endif //NETLIB_CALLBACKS_H
