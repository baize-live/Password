#include <ctime>
#include "password.h"
#include <cassert>
#include <sstream>
#include <iostream>

// 生成大奇数
BigInt generate_BigOddNum(int len) {
    static const char hex_table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    BigInt result("F");
    len >>= 2;
    // 十六进制数据,每位占4位二进制
    if (len) {
        std::ostringstream oss;
        for (size_t i = 0; i < len - 1; ++i) {
            oss << hex_table[rand() % 16];
        }
        // 最后一位为奇数
        oss << hex_table[1];
        result = oss.str();
    }
    return result;
}

BigInt generate_SmallerBigInt(const BigInt &val) {
    base_t t;
    do {
        t = rand();
    } while (t == 0);

    BigInt mod(t);
    BigInt ans = mod % val;
    if (ans == BigInt::ZERO) {
        ans = val - BigInt::ONE;
    }
    return ans;
}

// 素性检测
bool isPrime(const BigInt &num, const unsigned k) {
    assert(num != BigInt::ZERO);// 测试num是否为0
    if (num == BigInt::ONE) {
        // 1不是素数
        return false;
    } else if (num == BigInt::TWO) {
        // 2是素数
        return true;
    }

    BigInt temp = num - 1;
    BigInt::bit b(temp);    // 二进制数
    if (b.at(0) == 1) {
        return false;
    }
    // num-1 = 2^s*d
    size_t s = 0;    // 统计二进制末尾有几个0
    BigInt d(temp);
    for (size_t i = 0; i < b.size(); ++i) {
        if (!b.at(i)) {
            ++s;
            // 计算出d
            d = d.shiftRight(1);
        } else {
            break;
        }
    }

    // 测试k次
    for (size_t i = 0; i < k; ++i) {
        // 生成一个介于[1,num-1]之间的随机数a
        BigInt a = generate_SmallerBigInt(num);
        BigInt x = a.modPow(d, num);
        if (x == BigInt::ONE) {
            // 可能为素数
            continue;
        }
        bool ok = true;
        // 测试所有0<=j<s,a^(2^j*d) mod num != -1
        for (size_t j = 0; j < s && ok; ++j) {
            if (x == temp) {
                // 有一个相等,可能为素数
                ok = false;
            }
            x = x.mul(x).mod(num);
        }
        if (ok) {
            // 确实都不等,一定为合数
            return false;
        }
    }
    // 通过所有测试,可能为素数
    return true;
}

// 生成大素数
BigInt generate_BigPrime(int len) {
    // 先生成一个奇数
    BigInt ans = generate_BigOddNum(len);
    // 素性检测
    while (!isPrime(ans, 15)) {
        // 下一个奇数
        ans = ans.add(BigInt::TWO);
    }
    return ans;
}

// 生成密钥对
void generate_RSA_KEY(BigInt &e, BigInt &d, BigInt &n) {
    BigInt p = generate_BigPrime(512);
    BigInt q = generate_BigPrime(512);

    n = p * q;
    BigInt eul = (p - 1) * (q - 1);

    e = 65537;
    d = e.modInv(eul);
}

// 公钥加密
BigInt encrypt_rsa(const BigInt &clear, const pair<BigInt, BigInt> &key) {
    return clear.modPow(key.first, key.second);
}

// 私钥解密
BigInt decrypt_rsa(const BigInt &cipher, const pair<BigInt, BigInt> &key) {
    return cipher.modPow(key.first, key.second);
}
