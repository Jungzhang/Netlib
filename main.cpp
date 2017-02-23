#include <iostream>
#include <thread>
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "net/TcpClient.h"
#include "net/TcpConnection.h"

Netlib::EventLoop *g_loop;

void sendMessage(const Netlib::TcpConnectionPtr &conn) {
    std::string buf;

    while (std::cin >> buf) {
        if (conn->connected()) {
            conn->send(buf);
        } else {
            printf("发送失败，还未连接上服务器");
        }
    }
    g_loop->quit();
}

void onConnection(const Netlib::TcpConnectionPtr &conn) {
    if (conn->connected()) {
        std::thread t(sendMessage, conn);
        t.detach();
    } else {
        printf("关闭了 %s \n", conn->name().c_str());
    }
}


void onMessage(const Netlib::TcpConnectionPtr &conn, Netlib::Buffer *buffer, Netlib::TimeStamp re) {
    printf("%s\n", buffer->retrieveAllAsString().c_str());
}

int main() {

    Netlib::EventLoop loop;
    g_loop = &loop;
    Netlib::InetAddress serverAddr("localhost", 9001);
    Netlib::TcpClient client(&loop, serverAddr);

    client.setConnectionCallback(onConnection);
    client.setMessageCallback(onMessage);
    client.enableRetry();
    client.connect();

    loop.loop();

    std::cout << "Hello World!" << std::endl;

    return 0;
}