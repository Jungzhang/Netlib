/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_ACCEPTOR_H
#define NETLIB_ACCEPTOR_H

#include "Socket.h"
#include "Channel.h"

namespace Netlib{

    class EventLoop;
    class InetAddress;

    class Acceptor {
    public:
        typedef std::function<void(int sockfd, const InetAddress &)> NewConnectionCallback;

        Acceptor(EventLoop *loop, const InetAddress &listenAddr);

        void setNewConnectionCallback(const NewConnectionCallback &cb);

        bool listenning() const ;

        void listen();

    private:
        void handleRead();

        EventLoop *loop_;
        Socket acceptSocket_;
        Channel acceptChannel_;
        NewConnectionCallback newConnectionCallback_;
        bool listenning_;
    };
}

#endif //NETLIB_ACCEPTOR_H
