/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#ifndef NETLIB_LOGGER_H
#define NETLIB_LOGGER_H

#include <cstddef>
#include <algorithm>
#include <bits/unique_ptr.h>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace Netlib {

    // 一些内部类
    // 日志缓存区的内部类
    class LogBuff {

    public:
        LogBuff() { currentIdx_ = 0; }

        // 向buffer中写数据
        void append(const char *data, size_t len);

        // 获得Buffer中有效长度
        size_t size() const;

        // 获得Buffer剩余空间
        size_t avail() const;

        // 取得buffer中的数据
        const char *data() const;

        // 清空缓存
        void reset();

    private:
        static const size_t kBufferSize = 1024 * 1024 * 4;
        static void cookieStart();    // 标记函数,用于cookie
        static void cookieEnd();      // 标记函数,用于cookie
        char buffer_[kBufferSize];      // 存数据的地方
        size_t currentIdx_;
        // 该变量用一个函数的地址来标记其在内存中的地址，万一内存中还有未刷入磁盘的数据则便于在coredump文件中寻找日志
        void (*cookie)();
    };

    // 文件操作类,文件命名是按照文件名+时间构成+扩展名构成
    class File {
    public:
        // 构造函数
        File(size_t rollSize, std::string &path, std::string &filename, std::string &suffix);
        // 析构函数
        ~File();
        // 写入数据
        int append(const char *data, size_t len);
        // 判断文件描述符是否有效
        bool avail();

        void setWarningFunc(std::function<void(void)> func);

    private:
        // 滚动文件
        void rollFile();
    private:
        int fileFd_ = -1;
        size_t rollSize_;       // 0表示不滚动
        size_t currentSize_;
        std::string path_;
        std::string fileName_;
        std::string suffix_;
        int count = 0;
        std::function<void(void)> warningFunc_;           // 当文件打开失败时报警函数
    };


    class Logger {

    public:
        enum LogLevel {
            FATAL,  // 崩溃
            ERROR,  // 错误
            WARN,   // 警告
            INFO,   // 信息
            DEBUG,  // 调试信息
        };

    public:
        Logger(size_t rollSize = 1024 * 1024 * 1024);
        // 删除复制构造函数
        Logger(const Logger &) = delete;
        ~Logger();

        // 前台线程向buffer中写入日志
        void append(const char *message, size_t len, LogLevel level);

        // 设置队列长度
        void count(int count);

        // 设置报警函数
        void setWarningFunc(std::function<void(void)> fileWarning, std::function<void(void)> queueWarning);

        // 设置输出文件的信息,包括路径、文件名、扩展名
        void setLogFileInfo( std::string filename, std::string path = "./", std::string suffix = "log");

        // 停止后台线程,关闭日志
        void stop();

    private:
        std::string levelToString(LogLevel level) const;

        // 后台线程函数
        void backThreadFunc();

    private:
        static int count_;                                   // 队列的最大长度
        std::mutex mutex_;                                  // 全局锁
        std::condition_variable_any condition_;             // 条件变量
        std::unique_ptr<LogBuff> current_buffer_;           // 当前操作的buffer
        std::unique_ptr<LogBuff> next_buffer_;              // 下一个buffer
        std::vector<std::unique_ptr<LogBuff>> buffers_;     // 待写入文件的缓存区队列
        bool isRuning_;                                     // 后台线程是否运行,用于析构时使用
        size_t rollSize_;                                   // 日志滚动大小
        std::thread backThread_;                            // 后台线程
        static const std::string levelString[5];            // 日志等级的字符串
        std::function<void(void)> warningFunc_;           // 当队列达到最大值时的报警函数
        std::function<void(void)> warningFileFunc_;           // 当队列达到最大值时的报警函数
        std::string path_;
        std::string filename_;
        std::string suffix_;

    };
}


#endif //NETLIB_LOGGER_H
