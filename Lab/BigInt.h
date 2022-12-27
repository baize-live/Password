#ifndef LAB_BIGINT_H
#define LAB_BIGINT_H

#include <string>
#include <vector>

typedef long long long_t;
typedef unsigned base_t;

// 大数
class BigInt {
private:
    bool is_negative{};           // 是否为负
    std::vector<base_t> data;   // 按位数据存储,高位在后

    BigInt trim();              // 去掉高位无用的0
    static int hexToNum(char);         // 十六进制字符转换为十进制数
    BigInt divideAndRemainder(const BigInt &, BigInt &) const;// 大整数整除和取余

public:
    // 便于大整数运算的二进制处理类
    class bit {
    public:
        explicit bit(const BigInt &);           // 根据大整数初始化
        bool at(size_t);                        // 返回第i位二进制是否为1
        size_t size() const { return length; }  // 返回大整数对应的二进制位数
    private:
        size_t length;                          // 二进制的总位数
        std::vector<base_t> bit_vector;         // 二进制数据存储,每一个元素对应32位二进制
    };

    BigInt() : is_negative(false) { data.push_back(0); }
    BigInt(const BigInt &);        // 利用给定的大整数初始化
    BigInt(const std::string &);   // 利用给定的十六进制字符串初始化
    BigInt(const long_t &);        // 利用给定的long_t类型数据初始化
    ~BigInt() = default;
    int compare(const BigInt &) const;  // 比较运算
    bool equals(const BigInt &) const;  // 判断是否等于给定数
    std::string toString() const;       // 将大整数转换为十六进制字符串
    BigInt abs() const;                 // 求大整数的绝对值

    BigInt add(const BigInt &val);          // 大整数加法
    BigInt sub(const BigInt &val);          // 大整数减法
    BigInt mul(const BigInt &val) const;    // 大整数乘法
    BigInt div(const BigInt &val);          // 大整数整除
    BigInt rem(const BigInt &val);          // 大整数取余
    BigInt mod(const BigInt &val);          // 大整数取模
    BigInt pow(const BigInt &e);            // 大整数幂乘
    BigInt modPow(const BigInt &e, const BigInt &m) const;// 大整数幂模运算
    BigInt modInv(const BigInt &m);         // 用扩展欧几里得算法求乘法逆元
    BigInt shiftLeft(unsigned);             // 移位运算,左移
    BigInt shiftRight(unsigned);            // 移位运算,右移

    static const int base_bit = 5;              // 2^5=32,大整数每位存储的二进制位数
    static const int base_char = 8;             // 组成大整数的一位需要的十六进制位数
    static const int base_int = 32;             // 大整数一位对应的二进制位数
    static const unsigned base_num = 0xffffffff;// 截取低位的辅助
    static const int base_temp = 0x1f;          // 截取模32的余数的辅助
    static const BigInt ZERO;   // 大整数常量0
    static const BigInt ONE;    // 大整数常量1
    static const BigInt TWO;    // 大整数常量2
    static const BigInt TEN;    // 大整数常量10
    static BigInt valueOf(const long_t &);// 将给定数转换为大整数并返回

    BigInt &operator=(const long_t &num) { return (*this) = BigInt(num); }

    BigInt &operator=(const std::string &str) { return (*this) = BigInt(str); }

protected:
    // 以下运算符重载函数主要用于像基本类型一样使用大整数类型
    friend BigInt operator+(const BigInt &, const BigInt &);

    friend BigInt operator-(const BigInt &, const BigInt &);

    friend BigInt operator*(const BigInt &, const BigInt &);

    friend BigInt operator/(const BigInt &, const BigInt &);

    friend BigInt operator%(const BigInt &, const BigInt &);

    friend bool operator<(const BigInt &, const BigInt &);

    friend bool operator>(const BigInt &, const BigInt &);

    friend bool operator==(const BigInt &, const BigInt &);

    friend bool operator<=(const BigInt &, const BigInt &);

    friend bool operator>=(const BigInt &, const BigInt &);

    friend bool operator!=(const BigInt &, const BigInt &);

    // 重载版本,使其能用于long_t类型
    friend BigInt operator+(const BigInt &, const long_t &);

    friend BigInt operator-(const BigInt &, const long_t &);

    friend BigInt operator*(const BigInt &, const long_t &);

    friend BigInt operator/(const BigInt &, const long_t &);

    friend BigInt operator%(const BigInt &, const long_t &);

    friend bool operator<(const BigInt &, const long_t &);

    friend bool operator>(const BigInt &, const long_t &);

    friend bool operator==(const BigInt &, const long_t &);

    friend bool operator<=(const BigInt &, const long_t &);

    friend bool operator>=(const BigInt &, const long_t &);

    friend bool operator!=(const BigInt &, const long_t &);

    friend std::ostream &operator<<(std::ostream &, const BigInt &);

};

#endif //LAB_BIGINT_H
