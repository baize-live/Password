#include <bits/stdc++.h>

using namespace std;

typedef unsigned char UINT_8;
typedef unsigned short UINT_16;
typedef unsigned int UINT_32;

const int P[] = {1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16};
const int S[] = {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7};
const int S_Inv[] = {14, 3, 4, 8, 1, 12, 10, 15, 7, 13, 9, 6, 11, 2, 0, 5};
const int N = 4;
const int m = 4;
const int l = 4;
const int size = m * l;

UINT_16 S_Turn(UINT_16 byte, const int *s) {
    UINT_16 ret = 0;
    // S盒逆处理
    for (int j = 0; j < m; ++j) {
        ret += s[(byte >> (12 - 4 * j)) & 0xf] << (12 - 4 * j);
    }
    return ret;
}

UINT_16 P_Turn(UINT_16 byte, const int *p) {
    UINT_16 ret = 0;
    // S盒逆处理
    for (int j = 0; j < size; ++j) {
        ret += ((byte >> (size - j - 1) & 0x1) << (size - p[j]));
    }
    return ret;
}

UINT_16 encrypt_spn(UINT_32 key, UINT_16 byte) {
    for (int i = 0; i < N; ++i) {
        // 生成U
        byte = byte ^ (key >> (16 - 4 * i));
        // 生成V
        byte = S_Turn(byte, S);
        // 最后一轮不过P盒
        if (i != N - 1) {
            // P盒处理
            byte = P_Turn(byte, P);
        }
    }
    return byte ^ (UINT_16) key;
}

UINT_16 decrypt_spn(UINT_32 key, UINT_16 byte) {
    for (int i = N; i > 0; --i) {
        // 生成U
        byte = byte ^ (key >> (16 - 4 * i));
        // 第一轮不过P盒
        if (i != N) {
            // P盒处理
            byte = P_Turn(byte, P);
        }
        // 生成V
        byte = S_Turn(byte, S_Inv);
    }
    return byte ^ (UINT_16) (key >> 16);
}

int main() {
    string input_k, input_x;
    cin >> input_x >> input_k;
    // 原始密钥
    UINT_32 k = stoi(input_k, nullptr, 2);
    // 明文
    UINT_16 x = stoi(input_x, nullptr, 2);
    // 加密
    UINT_16 y = encrypt_spn(k, x);
    // 按需打印
    for (int i = 15; i >= 0; --i) {
        printf("%d", (y >> i) & 1);
    }
    return 0;
}