/***************************************************
* >Author: Jung
* >Email: jungzhang@xiyoulinux.org
* >Description: 一个统一 C style字符串和String字符串的类
***************************************************/

#ifndef NETLIB_STRINGPIECE_H
#define NETLIB_STRINGPIECE_H

class StringArg {
public:
    StringArg(const char *str) : str(str) {}
    StringArg(const std::string &str) : str(str.c_str()){}
    const char *c_str() const {
        return str;
    }

private:
    const char *str;
};

class StringPiece {
public:
    StringPiece() : ptr(nullptr), length(0){ }

    StringPiece(const char *str) : ptr(str), length(static_cast<int>(strlen(ptr))){ }

    StringPiece(const unsigned char *string) : ptr(reinterpret_cast<const char *>(string)),
                                               length(static_cast<int>(strlen(ptr))){ }

    StringPiece(const std::string &string) : ptr(string.data()),
                                             length(static_cast<int>(string.size())) { }

    StringPiece(const char *offset, int len) : ptr(offset), length(len) { }

    const char *data() const { return ptr;}

    int size() const { return length;}

    bool empty() { return length == 0;}

    const char *begin() const { return ptr;}

    const char *end() const { return ptr + length;}

    void clear() {ptr = nullptr; length = 0;}

    void set(const char *buffer, int len) {ptr = buffer; length = len;}

    void set(const char *str) {
        ptr = str;
        length = static_cast<int>(strlen(str));
    }

    void set(const void *buffer, int len) {
        ptr = reinterpret_cast<const char *>(buffer);
        length = len;
    }

    char operator[](int i) const { return ptr[i];}

    // 去除长度为n的前缀字符串
    void remove_prefix(int n) {
        ptr += n;
        length -= n;
    }

    // 去除长度为n的后缀字符串
    void remove_suffix(int n) {
        length -= n;
    }

    bool operator==(const StringPiece& x) const {
        return ((length == x.length) && (memcmp(ptr, x.ptr, length) == 0));
    }

    bool operator!=(const StringPiece& x) const {
        return !((*this) == x);
    }

    int compare(const StringPiece &x) const {
        int r = memcmp(ptr, x.ptr, length < x.length ? length : x.length);
        if (r == 0) {
            if (length < x.length) {
                r = -1;
            } else if (length > x.length) {
                r = +1;
            }
        }

        return r;
    }

    // 转化为string
    std::string as_string() const {
        return std::string(data(), size());
    }

    // 拷贝到string
    void copyToString(std::string *target) const {
        target->assign(ptr, length);  // 把ptr中前length个字符拷贝给target
    }

    // 本对象是不是以x对象内容为开始点
    bool start_with(const StringPiece &x) const {
        return ((length >= x.length) && (memcmp(ptr, x.ptr, x.length) == 0));
    }

private:
    const char *ptr;
    int length;
};

std::ostream& operator<<(std::ostream& o, const StringPiece& price);

#endif //NETLIB_STRINGPIECE_H
