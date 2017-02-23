/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_CONNECTOR_H
#define NETLIB_CONNECTOR_H

#include <atomic>
#include <memory>
#include <functional>
#include "InetAddress.h"
#include "TimerId.h"

namespace Netlib {

    class Channel;
    class EventLoop;

    class Connector {
    public:
        typedef std::function<void (int)> NewConnectionCallback;
        Connector(const Connector &) = delete;
        Connector &operator = (const Connector &) = delete;

        Connector(EventLoop *loop, const InetAddress &serverAddr);
        ~Connector();

        void setNewConnectionCallback(const NewConnectionCallback &cb);

        void start();
        void restart();
        void stop();

        const InetAddress &serverAddress() const ;

    private:
        enum States { kDisconnected, kConnecting, kConnected};
        static const int kInitRetryDelayMs = 500;               // 初始的重试时间间隔
        static const int kMaxRetDelayMs = 30 * 1000;                        // 最大的重试时间间隔

        void setState(States s);
        void startInLoop();
        void connect();
        void connecting(int sockfd);
        void handleWrite();
        void handleError();
        void retry(int sockfd);
        int removeAndResetChannel();
        void resetChannel();

    private:
        EventLoop *loop_;
        InetAddress serverAddr_;
        std::atomic_bool connect_;
        States state_;
        std::unique_ptr<Channel> channel_;
        NewConnectionCallback newConnectionCallback_;
        int retryDelayMs_;
        TimerId timerId_;
    };

    typedef std::unique_ptr<Connector> ConnectorPtr;
}

#endif //NETLIB_CONNECTOR_H
