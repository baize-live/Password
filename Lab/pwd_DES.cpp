#include "password.h"

// 置换IP
extern const int Turn_IP_Table_0[64];
// 逆置换IP
extern const int Turn_IP_Table_1[64];
// 压缩置换PC-1
extern const int Turn_PC_Table_1[56];
// 压缩置换PC-2
extern const int Turn_PC_Table_2[48];
// E表
extern const int E_Table[48];
// P表
extern const int P_Table[32];
// S盒
extern const int S_Table[8][4][16];
// 循环左移
extern const int Loop_Left_Shift[16];

// 子密钥 48bits 有效
UINT_64 KEY_48[16];

// 调整字节序
UINT_64 turn_byte(UINT_64 bits_64) {
    UINT_64 bits = bits_64;
    auto *bytes_8 = (UINT_8 *) &bits;
    for (int i = 0; i < sizeof(UINT_64) / 2; i++) {
        swap(bytes_8[i], bytes_8[7 - i]);
    }
    return bits;
}

// 置换IP算法 isTurn == TRUE 表示置换, FALSE 表示逆置换
UINT_64 turn_IP(UINT_64 bits_64, bool isTurn) {
    UINT_64 turn_IP_bits_64 = 0;
    if (isTurn) {
        for (int i: Turn_IP_Table_0) {
            turn_IP_bits_64 = (turn_IP_bits_64 << 1) + ((bits_64 >> (64 - i)) & 0x1);
        }
    } else {
        for (int i: Turn_IP_Table_1) {
            turn_IP_bits_64 = (turn_IP_bits_64 << 1) + ((bits_64 >> (64 - i)) & 0x1);
        }
    }
    return turn_IP_bits_64;
}

// 压缩置换密钥 isTurn == TRUE 表示PC_1, FALSE 表示PC_2
UINT_64 turn_PC(UINT_64 key, bool isTurn) {
    UINT_64 Key = 0;
    if (isTurn) {
        // key 64bits 有效
        for (int i: Turn_PC_Table_1) {
            Key = (Key << 1) + ((key >> (64 - i)) & 0x1);
        }
    } else {
        // key 56bits 有效
        for (int i: Turn_PC_Table_2) {
            Key = (Key << 1) + ((key >> (56 - i)) & 0x1);
        }
    }
    return Key;
}

// 生成子密钥
UINT_64 generate_sub_key(UINT_32 &C, UINT_32 &D, int times) {
    C = C << Loop_Left_Shift[times];
    C += (C >> 28);
    C &= 0xFFFFFFF;
    D = D << Loop_Left_Shift[times];
    D += (D >> 28);
    D &= 0xFFFFFFF;
    UINT_64 sub_key = 0;
    for (int i = 0; i < 28; ++i) {
        sub_key = (sub_key << 1) + ((C >> (27 - i)) & 0x1);
    }
    for (int i = 0; i < 28; ++i) {
        sub_key = (sub_key << 1) + ((D >> (27 - i)) & 0x1);
    }
    sub_key = turn_PC(sub_key, false);
    return sub_key;
}

// 生成所有密钥
void generate_key(UINT_64 key) {
    // 低 56bits 有效
    UINT_64 key_PC = turn_PC(key, true);
    // 低 28bits 有效
    UINT_32 C = 0;
    UINT_32 D = 0;
    // 为 C D 赋初值
    for (int i = 0; i < 28; ++i) {
        D = (D << 1) + ((key_PC >> (27 - i)) & 0x1);
    }
    for (int i = 0; i < 28; ++i) {
        C = (C << 1) + ((key_PC >> (55 - i)) & 0x1);
    }

    // 生成子密钥
    for (int i = 0; i < 16; ++i) {
        KEY_48[i] = generate_sub_key(C, D, i);
    }
}

// 选择扩展运算
UINT_64 E(UINT_32 bits_32) {
    UINT_64 bits_48 = 0;
    for (int i: E_Table) {
        bits_48 = (bits_48 << 1) + ((bits_32 >> (32 - i)) & 0x1);
    }
    return bits_48;
}

// S盒运算
UINT_32 S(UINT_64 bits_48) {
    UINT_32 bits_32 = 0;

    UINT_8 bits_2;
    UINT_8 bits_4;
    UINT_8 bits_6;
    for (int i = 0; i < 8; ++i) {
        bits_6 = (bits_48 >> (6 * (7 - i))) & 0x3F;
        bits_2 = ((bits_6 >> 5) << 1) + (bits_6 & 0x1);
        bits_4 = (bits_6 >> 1) & 0xF;
        bits_32 = (bits_32 << 4) + S_Table[i][bits_2][bits_4];
    }

    return bits_32;
}

// P置换
UINT_32 P(UINT_32 bits_32) {
    UINT_32 bits_32_new = 0;
    for (int i: P_Table) {
        bits_32_new = (bits_32_new << 1) + ((bits_32 >> (32 - i) & 0x1));
    }
    return bits_32_new;
}

// f函数 key为子密钥 48bits
UINT_32 f(UINT_32 bits_32, UINT_64 key_48) {
    UINT_64 bits_48 = E(bits_32) ^ key_48;
    bits_32 = S(bits_48);
    bits_32 = P(bits_32);
    return bits_32;
}

void handle_bits(UINT_8 *before_handle, UINT_8 *after_handle, int len, UINT_64 key_64, bool isEncrypt) {
    // 调整字节序
    key_64 = turn_byte(key_64);

    // 生成子密钥
    generate_key(key_64);

    // 补全64位
    int len_bits_8 = (len + 7) & ~7;

    auto *bits_8 = new UINT_8[len_bits_8];
    memcpy(bits_8, before_handle, len);
    memset(bits_8 + len, 0, len_bits_8 - len);

    // 转为64位数组
    auto *bits_64 = (UINT_64 *) bits_8;
    int len_bits_64 = len_bits_8 / 8;

    // 对数组中的各元素做处理
    for (int i = 0; i < len_bits_64; i++) {
        // 调整字节序
        bits_64[i] = turn_byte(bits_64[i]);
        // 置换IP
        bits_64[i] = turn_IP(bits_64[i], true);
        // 32bits的 左右密文
        UINT_32 L = (bits_64[i] >> 32) & 0xFFFFFFFF;
        UINT_32 R = bits_64[i] & 0xFFFFFFFF;
        UINT_32 temp_R;
        // 16轮轮换
        for (int j = 0; j < 16; ++j) {
            // 选择正确的密钥
            UINT_64 key_48 = isEncrypt ? KEY_48[j] : KEY_48[15 - j];
            temp_R = L ^ f(R, key_48);
            L = R;
            R = temp_R;
        }
        bits_64[i] = ((UINT_64) R << 32) + L;
        // 逆置换IP
        bits_64[i] = turn_IP(bits_64[i], false);
        // 调整字节序
        bits_64[i] = turn_byte(bits_64[i]);
    }

    // 完成加密
    memcpy(after_handle, bits_8, len);
    delete[] bits_8;
}

void encrypt_des(UINT_8 *clear, UINT_8 *cipher, int len, UINT_64 key_64) {
    handle_bits(clear, cipher, len, key_64, true);
}

void decrypt_des(UINT_8 *cipher, UINT_8 *clear, int len, UINT_64 key_64) {
    handle_bits(cipher, clear, len, key_64, false);
}

const int Turn_IP_Table_0[64] = {
        58, 50, 42, 34, 26, 18, 10, 2, 60,
        52, 44, 36, 28, 20, 12, 4,
        62, 54, 46, 38, 30, 22, 14, 6,
        64, 56, 48, 40, 32, 24, 16, 8,
        57, 49, 41, 33, 25, 17, 9, 1,
        59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5,
        63, 55, 47, 39, 31, 23, 15, 7
};
const int Turn_IP_Table_1[64] = {
        40, 8, 48, 16, 56, 24, 64, 32,
        39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30,
        37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28,
        35, 3, 43, 11, 51, 19, 59, 27,
        34, 2, 42, 10, 50, 18, 58, 26,
        33, 1, 41, 9, 49, 17, 57, 25
};
const int Turn_PC_Table_1[56]{
        57, 49, 41, 33, 25, 17, 9, 1,
        58, 50, 42, 34, 26, 18, 10, 2,
        59, 51, 43, 35, 27, 19, 11, 3,
        60, 52, 44, 36, 63, 55, 47, 39,
        31, 23, 15, 7, 62, 54, 46, 38,
        30, 22, 14, 6, 61, 53, 45, 37,
        29, 21, 13, 5, 28, 20, 12, 4
};
const int Turn_PC_Table_2[48]{
        14, 17, 11, 24, 1, 5, 3, 28,
        15, 6, 21, 10, 23, 19, 12, 4,
        26, 8, 16, 7, 27, 20, 13, 2,
        41, 52, 31, 37, 47, 55, 30, 40,
        51, 45, 33, 48, 44, 49, 39, 56,
        34, 53, 46, 42, 50, 36, 29, 32
};

const int E_Table[48]{
        32, 1, 2, 3, 4, 5, 4, 5,
        6, 7, 8, 9, 8, 9, 10, 11,
        12, 13, 12, 13, 14, 15, 16, 17,
        16, 17, 18, 19, 20, 21, 20, 21,
        22, 23, 24, 25, 24, 25, 26, 27,
        28, 29, 28, 29, 30, 31, 32, 1

};

const int P_Table[32]{
        16, 7, 20, 21, 29, 12, 28, 17,
        1, 15, 23, 26, 5, 18, 31, 10,
        2, 8, 24, 14, 32, 27, 3, 9,
        19, 13, 30, 6, 22, 11, 4, 25
};

const int S_Table[8][4][16]{
        {
                {14, 4,  13, 1,  2,  15, 11, 8,  3,  10, 6,  12, 5,  9,  0,  7},
                {0,  15, 7,  4,  14, 2,  13, 1,  10, 6, 12, 11, 9,  5,  3,  8},
                {4,  1,  14, 8,  13, 6,  2,  11, 15, 12, 9,  7,  3,  10, 5,  0},
                {15, 12, 8,  2,  4,  9,  1,  7,  5,  11, 3,  14, 10, 0, 6,  13},
        },
        {
                {15, 1,  8,  14, 6,  11, 3,  4,  9,  7,  2,  13, 12, 0,  5,  10},
                {3,  13, 4,  7,  15, 2,  8,  14, 12, 0, 1,  10, 6,  9,  11, 5},
                {0,  14, 7,  11, 10, 4,  13, 1,  5,  8,  12, 6,  9,  3,  2,  15},
                {13, 8,  10, 1,  3,  15, 4,  2,  11, 6,  7,  12, 0,  5, 14, 9},
        },
        {
                {10, 0,  9,  14, 6,  3,  15, 5,  1,  13, 12, 7,  11, 4,  2,  8},
                {13, 7,  0,  9,  3,  4,  6,  10, 2,  8, 5,  14, 12, 11, 15, 1},
                {13, 6,  4,  9,  8,  15, 3,  0,  11, 1,  2,  12, 5,  10, 14, 7},
                {1,  10, 13, 0,  6,  9,  8,  7,  4,  15, 14, 3,  11, 5, 2,  12},
        },
        {
                {7,  13, 14, 3,  0,  6,  9,  10, 1,  2,  8,  5,  11, 12, 4,  15},
                {13, 8,  11, 5,  6,  15, 0,  3,  4,  7, 2,  12, 1,  10, 14, 9},
                {10, 6,  9,  0,  12, 11, 7,  13, 15, 1,  3,  14, 5,  2,  8,  4},
                {3,  15, 0,  6,  10, 1,  13, 8,  9,  4,  5,  11, 12, 7, 2,  14},
        },
        {
                {2,  12, 4,  1,  7,  10, 11, 6,  8,  5,  3,  15, 13, 0,  14, 9},
                {14, 11, 2,  12, 4,  7,  13, 1,  5,  0, 15, 10, 3,  9,  8,  6},
                {4,  2,  1,  11, 10, 13, 7,  8,  15, 9,  12, 5,  6,  3,  0,  14},
                {11, 8,  12, 7,  1,  14, 2,  13, 6,  15, 0,  9,  10, 4, 5,  3},
        },
        {
                {12, 1,  10, 15, 9,  2,  6,  8,  0,  13, 3,  4,  14, 7,  5,  11},
                {10, 15, 4,  2,  7,  12, 9,  5,  6,  1, 13, 14, 0,  11, 3,  8},
                {9,  14, 15, 5,  2,  8,  12, 3,  7,  0,  4,  10, 1,  13, 11, 6},
                {4,  3,  2,  12, 9,  5,  15, 10, 11, 14, 1,  7,  6,  0, 8,  13},
        },
        {
                {4,  11, 2,  14, 15, 0,  8,  13, 3,  12, 9,  7,  5,  10, 6,  1},
                {13, 0,  11, 7,  4,  9,  1,  10, 14, 3, 5,  12, 2,  15, 8,  6},
                {1,  4,  11, 13, 12, 3,  7,  14, 10, 15, 6,  8,  0,  5,  9,  2},
                {6,  11, 13, 8,  1,  4,  10, 7,  9,  5,  0,  15, 14, 2, 3,  12},
        },
        {
                {13, 2,  8,  4,  6,  15, 11, 1,  10, 9,  3,  14, 5,  0,  12, 7},
                {1,  15, 13, 8,  10, 3,  7,  4,  12, 5, 6,  11, 0,  14, 9,  2},
                {7,  11, 4,  1,  9,  12, 14, 2,  0,  6,  10, 13, 15, 3,  5,  8},
                {2,  1,  14, 7,  4,  10, 8,  13, 15, 12, 9,  0,  3,  5, 6,  11},
        }
};

const int Loop_Left_Shift[16]{
        1, 1, 2, 2, 2, 2, 2, 2,
        1, 2, 2, 2, 2, 2, 2, 1
};