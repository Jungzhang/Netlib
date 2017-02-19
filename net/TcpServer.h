/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_TCPSERVER_H
#define NETLIB_TCPSERVER_H

#include <map>
#include <bits/unique_ptr.h>
#include "InetAddress.h"
#include "Callbacks.h"

namespace Netlib{

    class EventLoop;
    class Acceptor;

    class TcpServer {
    public:
        // 不可拷贝的类
        TcpServer(const TcpServer &) = delete;
        TcpServer &operator=(const TcpServer &) = delete;
        // 构造和析构函数
        TcpServer(EventLoop *loop, const InetAddress &listenAddr);
        ~TcpServer();

        // 启动
        void start();
        // 设置连接回调
        void setConnectionCallback(const ConnectionCallback &cb);
        // 设置消息回调
        void setMessageCallback(const MessageCallback &cb);


    private:
        void newConnection(int sockfd, const InetAddress &peerAdrr);
        void removeConnection(const TcpConnectionPtr &conn);
        typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    private:
        EventLoop *loop_;
        const std::string name_;
        std::unique_ptr<Acceptor> acceptor_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        bool started_;
        int nextConnId_;
        ConnectionMap connections_;
    };
};

#endif //NETLIB_TCPSERVER_H
