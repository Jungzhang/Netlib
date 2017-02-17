/*************************************************************************
	> File Name: testAcceptor.cpp
	> Author: Jung
	> Mail: jungzhang@xiyoulinux.org
	> Created Time: 2017年02月17日 星期五 22时21分02秒
	> Description:
 ************************************************************************/
#include "../../net/SocketOps.h"
#include "../../net/EventLoop.h"
#include "../../net/Acceptor.h"
#include "../../net/InetAddress.h"

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