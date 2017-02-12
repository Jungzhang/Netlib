/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <iostream>
#include <zconf.h>
#include <fcntl.h>
#include <assert.h>
#include <cstring>
#include "Logger.h"

namespace Netlib {

    namespace NetlibTemp {
        // 一个空实现
        void initWarningFunc(void *arg) {
//            std::cerr << "The queue is full" << std::endl;
        }

        void initWarningFileFunc(void *buffer) {
//            std::cerr << "The file write filed" << std::endl;
        };
    }

    int Logger::count_ = 25;
    const std::string Logger::levelString[5] = {" FATAL", " ERROR", " WARN ", " INFO ", " DEBUG"};

    const char *LogBuff::data() const {
        return buffer_;
    }

    void LogBuff::append(const char *data, size_t len) {
        std::copy(data, data + len, buffer_ + currentIdx_);
        currentIdx_ += len;
    }

    size_t LogBuff::size() const {
        return currentIdx_;
    }

    void LogBuff::reset() {
        currentIdx_ = 0;
    }

    void LogBuff::cookieStart() {}

    void LogBuff::cookieEnd() {}

    size_t LogBuff::avail() const {
        return kBufferSize - currentIdx_;
    }

    File::File(size_t rollSize, std::string &path, std::string &fileName, std::string &suffix)
            : rollSize_(rollSize), currentSize_(0), fileFd_(-1) {
        assert(path.size());
        assert(fileName.size());
        path_ = std::move(path);
        fileName_ = std::move(fileName);
        suffix_ = std::move(suffix);
        fileFd_ = ::open((path_ + fileName_ + "-" + std::to_string(count) + "." + suffix_).c_str(),
                         O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    }

    File::~File() {
        if (fileFd_ >= 0) {
            ::close(fileFd_);
        }
    }

    bool File::avail() {
        return fileFd_ >= 0;
    }

    int File::append(const char *data, size_t len) {
        size_t resultSize;
        // roolSize为0则表示不滚动
        if (rollSize_ && (currentSize_ + len > rollSize_)) {
            rollFile();
        }
        if (avail()) {
            resultSize = static_cast<size_t >(::write(fileFd_, data, len));
            currentSize_ += resultSize;
        } else {
            warningFunc_;
            return -1;
        }
        return static_cast<int>(resultSize);
    }

    void File::rollFile() {
        if (fileFd_ >= 0) {
            ::close(fileFd_);
        }
        ++count;
        fileFd_ = ::open((path_ + fileName_ + "-" + std::to_string(count) + "." + suffix_).c_str(),
                         O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
        currentSize_ = 0;
    }

    void File::setWarningFunc(std::function<void(void *)> func) {
        warningFunc_ = func;
    }

    std::string Logger::levelToString(LogLevel level) const {
        return levelString[level];
    }

    void Logger::stop() {
        isRuning_ = false;
        backThread_.join();
    }

    Logger::Logger(size_t rollSize) : rollSize_(rollSize), backThread_(std::bind(&Logger::backThreadFunc, this)),
                                      isRuning_(true),
                                      current_buffer_(new LogBuff), next_buffer_(new LogBuff),
                                      warningFunc_(std::bind(NetlibTemp::initWarningFunc, nullptr)) {}

    Logger::~Logger() {
        stop();
    }

    void Logger::count(int count) {
        count_ = count;
    }

    void Logger::setWarningFunc(
            std::function<void(void *)> fileWraning = std::bind(NetlibTemp::initWarningFileFunc, nullptr),
            std::function<void(void *)> queueWraning = std::bind(NetlibTemp::initWarningFunc, nullptr)) {
        warningFunc_ = queueWraning;
        warningFileFunc_ = fileWraning;
    }

    void Logger::setLogFileInfo(std::string filename, std::string path, std::string suffix) {
        path_ = std::move(path);
        filename_ = std::move(filename);
        suffix_ = std::move(suffix);
    }

// 向缓存区中添加数据
    void Logger::append(const char *message, size_t len, LogLevel level) {
        if (LogLevel::FATAL <= level && level <= LogLevel::DEBUG) {
            std::lock_guard<std::mutex> lock(mutex_);
            // 如果当前缓存区容纳不下所有的数据则需要选择下一个缓存区
            // 加7是因为日志级别占6个字节,后边的\n占一个字节
            if (current_buffer_->avail() < len + 7) {
                // 将当前缓存区插入队列
                if (buffers_.size() < count_) {
                    buffers_.push_back(std::move(current_buffer_));
                } else {
                    // 通知报警,默认为一个空函数,即丢掉buffer中的数据
                    warningFunc_;
                }
                // 如果下一个缓存区可用则选择为下一个缓存区,否则就重新分配缓存区
                if (next_buffer_) {
                    current_buffer_ = std::move(next_buffer_);
                } else {
                    current_buffer_.reset(new LogBuff);
                }
            }
            current_buffer_->append(message, len);      // 添加日志内容
            current_buffer_->append(levelToString(level).c_str(), 6);   // 添加级别
            current_buffer_->append("\n", 1);           // 添加\n

            // 如果队列不为空则唤醒后台线程
            if (buffers_.size() != 0) {
                condition_.notify_one();
            }
        }
    }

    void Logger::backThreadFunc() {
        std::unique_ptr<LogBuff> buffer1(new LogBuff);
        std::unique_ptr<LogBuff> buffer2(new LogBuff);
        // 为提高效率使用了writeBuffers这个临时空间
        // 用来将buffers_中的数据一次性移动出来交给后台线程,以此来减少临界区的代码
        std::vector<std::unique_ptr<LogBuff>> writeBuffers;
        File output(rollSize_, path_, filename_, suffix_);
        output.setWarningFunc(warningFileFunc_);

        while (isRuning_) {
            // 使用此大括号减少锁的生存周期,提高效率
            {
                std::lock_guard<std::mutex> locker(mutex_);
                // 如果队列为空则阻塞当前线程,并设置超时等待时间为3s,即3s无论如何要将日志刷新一次到磁盘中
                if (buffers_.empty()) {
                    condition_.wait_for(mutex_, std::chrono::seconds(1));
                }
                // 将现在的buffer加入队列
                buffers_.push_back(std::move(current_buffer_));
                // 当前缓存区更改为buffer1
                current_buffer_ = std::move(buffer1);
                writeBuffers.swap(buffers_);    // 交换队列
                // 如果备选缓存区为空则将buffer2移动给它
                if (!next_buffer_) {
                    next_buffer_ = std::move(buffer2);
                }
            }

            // 将队列中内容写入文件
            for (auto &tmpBuffer : writeBuffers) {
                output.append(tmpBuffer->data(), tmpBuffer->size());
            }

            // 填充后台线程的两个buffer
            if (writeBuffers.size() > 2) {
                writeBuffers.resize(2);
            }

            // 重置后台线程buffer1
            if (!buffer1.get()) {
                assert(!writeBuffers.empty());
                buffer1 = std::move(writeBuffers[0]);
                buffer1->reset();
            }

            // 重置后台线程buffer2
            if (!buffer2.get()) {
                assert(!writeBuffers.empty());
                buffer2 = std::move(writeBuffers[1]);
                buffer2->reset();
            }

            // 清除临时变量writeBuffers中的内容
            writeBuffers.clear();
        }

    }
}
