/*************************************************************************
	> File Name: testMuiltThreadServer.cpp
	> Author: Jung
	> Mail: jungzhang@xiyoulinux.org
	> Description:
 ************************************************************************/
#include "../../net/Callbacks.h"
#include "../../net/TcpConnection.h"
#include "../../net/EventLoop.h"
#include "../../net/TcpServer.h"

void onConnection(const Netlib::TcpConnectionPtr &conn) {
    if (conn->connected()) {
        printf("一个新连接，name: %s  IP：%s\n", conn->name().c_str()
                , conn->peerAddress().toHostPort().c_str());
    } else {
        printf("一个连接离开了，name: %s\n", conn->name().c_str());
    }
}

void onMessage(const Netlib::TcpConnectionPtr &conn, Netlib::Buffer *buf, Netlib::TimeStamp re) {
    std::string str(std::move(buf->retrieveAllAsString()));
    printf("收到来自 %s 的消息：%s\n", conn->peerAddress().toHostPort().c_str()
            , str.c_str());
    conn->send(str);
}

int main() {

    Netlib::InetAddress inetAddress(9001);

    Netlib::EventLoop eventLoop;
    eventLoop.setTimeoutMs(-1);
    Netlib::TcpServer tcpServer(&eventLoop, inetAddress);
    tcpServer.setConnectionCallback(onConnection);
    tcpServer.setMessageCallback(onMessage);
    tcpServer.setThreadNum(4);      // 设置多线程数,如果不设置就是单线程
    tcpServer.start();
    eventLoop.loop();

    return 0;
}

