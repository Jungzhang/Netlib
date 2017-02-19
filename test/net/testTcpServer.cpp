/*************************************************************************
	> File Name: testTcpServer.cpp
	> Author: Jung
	> Mail: jungzhang@xiyoulinux.org
	> Description:
 ************************************************************************/
#include <iostream>
#include <zconf.h>
#include "../../net/Callbacks.h"
#include "../../net/TcpConnection.h"
#include "../../net/EventLoop.h"
#include "../../net/TcpServer.h"
#include "../../net/Buffer.h"

void onConnection(const Netlib::TcpConnectionPtr &conn) {
    if (conn->connected()) {
        printf("一个新的连接, name : %s  form %s\n", conn->name().c_str()
                , conn->peerAddress().toHostPort().c_str());
    } else {
        printf("一个连接离开了, name:%s\n", conn->name().c_str());
    }
}

void onMessage(const Netlib::TcpConnectionPtr &conn, Netlib::Buffer *buf, Netlib::TimeStamp re) {
    printf("received %zd bytes from connection : %s\n", buf->readableBytes()
            , conn->peerAddress().toHostPort().c_str());
    printf("the data : %s\n", buf->retrieveAllAsString().c_str());
}


int main() {

    printf("main() : pid = %d\n", getpid());
    Netlib::InetAddress listenAddr(9001);
    Netlib::EventLoop loop;

    Netlib::TcpServer server(&loop, listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();

    loop.loop();

    return 0;
}