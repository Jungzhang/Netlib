#include <iostream>
#include <zconf.h>
#include "net/InetAddress.h"
#include "net/SocketOps.h"
#include "net/EventLoop.h"
#include "net/Acceptor.h"

void newConnection(int fd, const Netlib::InetAddress &peeraddr) {
    printf("new client connected ip and port : %s\n", peeraddr.toHostPort().c_str());
    ::write(fd, "Hello World\n", 12);
    Netlib::sockets::close(fd);
}

int main() {

    printf("main() pid = %d\n", getpid());

    Netlib::InetAddress listenAddr(9001);
    Netlib::EventLoop loop;

    Netlib::Acceptor acceptor(&loop, listenAddr);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();

    loop.loop();


//    std::cout << "Hello World!" << std::endl;

    return 0;
}