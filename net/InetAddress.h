/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_INETADDRESS_H
#define NETLIB_INETADDRESS_H

#include <cstdint>
#include <string>
#include <netinet/in.h>

namespace Netlib{
    class InetAddress {
    public:
        // 构造函数,传入端口
        explicit InetAddress(uint16_t port);
        // 传入字符串形式的ip地址和端口
        InetAddress(const std::string &ip, uint16_t port);
        // 传入Ip的结构体
        InetAddress(const struct sockaddr_in &addr);
        // 返回字符串形式的端口
        std::string toHostPort() const ;
        // 返回地址的结构体
        const struct sockaddr_in &getSockAddrInet() const ;
        // 设置IP地址
        void setSockAddrInet(const struct sockaddr_in &addr);

    private:
        struct sockaddr_in addr_;
    };
}

#endif //NETLIB_INETADDRESS_H
