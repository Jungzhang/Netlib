/*********************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description:
**********************************************/

#include <algorithm>
#include "LogStream.h"

template <int SIZE>
void FixedBuffer<SIZE>::cookiesEnd() {

}

template <int SIZE>
void FixedBuffer<SIZE>::cookiesStart() {

}

template <int SIZE>
const char* FixedBuffer<SIZE>::debugString() {
    *cur = '\0';
    return data_;
}

void LogStream::append(const char *data, int len) {
    buffer_.append(data, len);
}

const LogStream::FixBuffer &LogStream::buffer() const {
    return buffer_;
}

LogStream& LogStream::operator<<(bool v) {
    buffer_.append(v ? "1" : "0", 1);
    return *this;
}

LogStream& LogStream::operator<<(char v) {
    buffer_.append(&v, 1);
    return *this;
}

LogStream& LogStream::operator<<(const char *v) {

    if (v) {
        buffer_.append(v, ::strlen(v));
    } else {
        buffer_.append("(null)", 6);
    }

    return *this;
}

template <int SIZE>
LogStream& LogStream::operator<<(const FixedBuffer<SIZE> &v) {
    *this << v.toStringPiece();
    return *this;
}

LogStream& LogStream::operator<<(const std::string &v) {
    buffer_.append(v.c_str(), v.size());
    return *this;
}

LogStream& LogStream::operator<<(const StringPiece &v) {
    buffer_.append(v.data(), v.size());
    return *this;
}

LogStream& LogStream::operator<<(const unsigned char *v) {
    *this << reinterpret_cast<const char *>(v); // 可以任意转换的reinterpret_cast
    return *this;
}

LogStream& LogStream::operator<<(const void *v) {
    return operator<<(static_cast<const char *>(v));
}

LogStream& LogStream::operator<<(double v) {

    // 精度判断
    if (buffer_.avail() >= kMaxNumericSize) {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
        buffer_.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(float v) {
    *this << static_cast<double >(v);
    return *this;
}

LogStream& LogStream::operator<<(int v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(short v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short v) {
    formatInteger(v);
    return *this;
}

template <typename T>
size_t convert(char buf[], T value) {
    T i = value;
    char *p = buf;
    const char *zero = "0123456789";
    do {
        int lsd = static_cast<int> (i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0) {
        *p++ = '-';
    }
    p = '\0';

    std::reverse(buf, p);

    return p - buf;
}

void LogStream::resetBuffer() {
    buffer_.reset();
}

template <typename T>
void LogStream::formatInteger(T v) {
    if (buffer_.avail() >= kMaxNumericSize) {
        size_t len = convert(buffer_.current(), v);
        buffer_.add(len);
    }
}
