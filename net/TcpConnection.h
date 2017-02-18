/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_TCPCONNECTION_H
#define NETLIB_TCPCONNECTION_H

#include <string>
#include "InetAddress.h"
#include "Callbacks.h"

namespace Netlib {

    class EventLoop;

    class Channel;

    class Socket;

    class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
    public:
        TcpConnection(const TcpConnection &) = delete;

        TcpConnection &operator=(const TcpConnection &) = delete;

        TcpConnection(EventLoop *loop, const std::string &name, int sockfd, const InetAddress &localAddr,
                      const InetAddress &peerAddr);
        ~TcpConnection();

        EventLoop *getLoop() const ;
        const std::string &name() const ;
        const InetAddress &localAddress() const ;
        const InetAddress &peerAddress() const ;
        bool connected() const ;

        void setConnectionCallback(const ConnectionCallback &cb);
        void setMessageCallback(const MessageCallback &cb);

        void connectEstablished();

    private:
        enum StateE { kConnecting, kConnected};

        void setState(StateE s);
        void handleRead();

        EventLoop *loop_;
        std::string name_;
        StateE state_;
        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;
        InetAddress localAddr_;
        InetAddress peerAddr_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
    };
}

#endif //NETLIB_TCPCONNECTION_H
