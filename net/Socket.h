/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_SOCKET_H
#define NETLIB_SOCKET_H

namespace Netlib{

    class InetAddress;

    class Socket {
    public:
        Socket(const Socket &) = delete;
        Socket &operator=(const Socket &) = delete;

        explicit Socket(int sockfd);

        ~Socket();

        int fd () const ;

        // 绑定
        void bindAddress(const InetAddress &localAddr);
        // 转换为listen fd
        void listen();
        // 接受连接
        int accept(InetAddress *peeraddr);
        // 设置端口可重用
        void setReuseAddr(bool on);

        void shutdownWrite();

    private:
        const int sockfd_;
    };
}

#endif //NETLIB_SOCKET_H
