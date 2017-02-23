/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_TCPCLIENT_H
#define NETLIB_TCPCLIENT_H

#include <memory>
#include "Callbacks.h"
#include "EventLoop.h"
#include "InetAddress.h"

namespace Netlib {

    class Connector;
    typedef std::shared_ptr<Connector> ConnectorPtr;

    class TcpClient {
    public:
        TcpClient(const TcpClient &) = delete;
        TcpClient &operator=(const TcpClient &) = delete;

        TcpClient(EventLoop *loop, const InetAddress &serverAddr);
        ~TcpClient();

        void connect();
        void disconnect();
        void stop();

        TcpConnectionPtr connection() const ;

        bool retry() const ;
        void enableRetry();

        void setConnectionCallback(const ConnectionCallback &cb);
        void setMessageCallback(const MessageCallback &cb);
        void setWriteCompleteCallback(const WriteCompleteCallback &cb);

    private:
        void newConnection(int sockfd);
        void removeConnection(const TcpConnectionPtr &conn);

    private:
        EventLoop *loop_;
        ConnectorPtr connector_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        std::atomic_bool retry_;
        std::atomic_bool connect_;
        int nextConnId_;
        mutable std::mutex mutex_;
        TcpConnectionPtr connection_;
    };
}

#endif //NETLIB_TCPCLIENT_H
