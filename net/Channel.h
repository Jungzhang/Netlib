/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_CHANNEL_H
#define NETLIB_CHANNEL_H

#include <sys/poll.h>
#include <functional>

namespace Netlib{
    class EventLoop;
    class Channel {
    public:

        // 回调事件的数据类型
        typedef std::function<void(void)> EventCallback;
        typedef std::function<void(Netlib::TimeStamp)> ReadEventCallback;

        // 不可复制的类
        Channel(const Channel&) = delete;
        Channel&operator=(const Channel &) = delete;

        // 构造函数
        Channel(EventLoop *loop, int fd);

        ~Channel();

        // 事件分发
        void handleEvent(TimeStamp receiveTime);

        // 设置回调
        void setReadCallback(const ReadEventCallback &cb);
        void setWriteCallback(const EventCallback &cb);
        void serErrorCallback(const EventCallback &cb);
        void setCloseCallback(const EventCallback &cb);

        int fd() const ;                // 返回当前Channel所管理的fd
        int events() const ;            // 返回注册的事件
        void setRevents(int revt);      // 设置注册的事件
        bool isNoneEvent() const ;      // 注册的是否是空事件

        void enableReading();           // 设置为可读
        void enableWriting();           // 设置为可写
        void disableReading();          // 设置为不可读
        void disableWriting();          // 设置为不可写
        void disableAll();              // 将所有的事件都取消

        // Poller
        int index();
        void setIndex(int idx);

        // 返回Channel所属的EventLoop
        EventLoop *ownerLoop();

    private:
        void update();                  // 更新

    private:

        // 无事件  读事件   写事件
        static const int kNoneEvent = 0;
        static const int kReadEvent = POLLIN | POLLPRI;
        static const int kWriteEvent = POLLOUT;

        ReadEventCallback readCallback_;    // 读事件回调
        EventCallback writeCallback_;   // 写事件回调
        EventCallback errorCallback_;   // 错误事件回调
        EventCallback closeCallback_;   // 关闭连接的事件回调

        bool eventHandling_;            // eventHandle是否正在运行

        EventLoop *eventLoop_;          // 事件循环
        const int fd_;                  // 所管理的文件描述符
        int events_;                    // 注册事件
        int revents_;                   // 发生事件
        int index_;                     // 记住自己在poll函数中fds参数的下标
    };
}

#endif //NETLIB_CHANNEL_H
