#include "password.h"

extern const UINT_32 R_CON[];
extern const UINT_32 S_Table[16][16];
extern const UINT_32 S_Table_Inv[16][16];
extern const UINT_8 ColMix_Table[4][4];
extern const UINT_8 ColMix_Table_Inv[4][4];
extern map<int, int> Nk_map_Nr;
extern map<int, vector<int>> Nb_map_RowShiftNum;

// 只处理128位明文
const int handle_bytes_len = 16;
int Nb, Nk, Nr;

// GF上的二元运算
int GFMul2(int s) {
    int result = s << 1;
    int a7 = result & 0x00000100;

    if (a7 != 0) {
        result = result & 0x000000ff;
        result = result ^ 0x1b;
    }

    return result;
}

int GFMul4(int s) {
    return GFMul2(GFMul2(s));
}

int GFMul8(int s) {
    return GFMul2(GFMul4(s));
}

int GFMul(int n, int s) {
    int result;
    switch (n) {
        case 1:
            result = s;
            break;
        case 2:
            result = GFMul2(s);
            break;
        case 3:
            result = GFMul2(s) ^ s;
            break;
        case 0x9:
            result = GFMul8(s) ^ s;
            break;
        case 0xb:
            result = GFMul8(s) ^ GFMul2(s) ^ s;
            break;
        case 0xd:
            result = GFMul8(s) ^ GFMul4(s) ^ s;
            break;
        case 0xe:
            result = GFMul8(s) ^ GFMul4(s) ^ GFMul2(s);
            break;
        default :
            result = s;
            break;
    }
    return result;
}

// 字节代换
void byte_sub(UINT_8 *bytes, int len, bool isEncrypt) {
    int row;
    int col;
    for (int i = 0; i < len; ++i) {
        row = (bytes[i] >> 4) & 0xF;
        col = bytes[i] & 0xF;
        bytes[i] = isEncrypt ? S_Table[row][col] : S_Table_Inv[row][col];
    }
}

// 行移位
void shift_row(UINT_8 *bytes, bool isEncrypt) {
    auto vec = Nb_map_RowShiftNum[Nb];
    auto *temp = new UINT_8[Nb];
    for (int i = 1; i < 4; ++i) {
        for (int j = 0; j < Nb; ++j) {
            temp[j] = bytes[i + j * 4];
        }
        for (int j = 0; j < Nb; ++j) {
            if (isEncrypt) {
                int index = j + vec[i - 1];
                bytes[i + j * 4] = temp[(index < Nb) ? index : index - Nb];
            } else {
                int index = j - vec[i - 1];
                bytes[i + j * 4] = temp[(index >= 0) ? index : index + Nb];
            }
        }
    }
    delete[] temp;
}

// 列混合
void mix_col(UINT_8 *bytes, bool isEncrypt) {
    auto *temp = new UINT_8[4];
    UINT_8 sum;
    for (int i = 0; i < Nb; i++) {
        for (int j = 0; j < 4; ++j) {
            temp[j] = bytes[Nb * i + j];
        }
        for (int j = 0; j < 4; ++j) {
            sum = 0;
            for (int k = 0; k < 4; k++) {
                sum ^= GFMul(isEncrypt ? ColMix_Table[j][k] : ColMix_Table_Inv[j][k], temp[k]);
            }
            bytes[Nb * i + j] = sum;
        }
    }
    delete[] temp;
}

// 密钥加
void add_round_key(UINT_8 *bytes, const UINT_32 *key_extend) {
    UINT_8 temp;
    for (int i = 0; i < handle_bytes_len; i++) {
        temp = ((UINT_8 *) &key_extend[i / Nb])[i % Nb];
        bytes[i] ^= temp;
    }
}

// 加密轮函数
void round_encrypt(UINT_8 *bytes, UINT_32 *key_extend, int round_num) {
    if (round_num != 0) {
        byte_sub(bytes, handle_bytes_len, true);
        shift_row(bytes, true);
        if (round_num != Nr) {
            mix_col(bytes, true);
        }
    }
    add_round_key(bytes, key_extend);
}

// 解密轮函数
void round_decrypt(UINT_8 *bytes, UINT_32 *key_extend, int round_num) {
    if (round_num != Nr) {
        byte_sub(bytes, handle_bytes_len, false);
        shift_row(bytes, false);
        if (round_num != 0) {
            mix_col(bytes, false);
            mix_col((UINT_8 *) key_extend, false);
        }
    }
    add_round_key(bytes, key_extend);
}

// 调整字节序
UINT_32 turn_byte(UINT_32 bits_32) {
    UINT_32 bits = bits_32;
    auto *bytes_8 = (UINT_8 *) &bits;
    for (int i = 0; i < sizeof(UINT_32) / 2; i++) {
        swap(bytes_8[i], bytes_8[3 - i]);
    }
    return bits;
}

// 字节置换
UINT_32 sub_byte(UINT_32 dword) {
    auto *bytes = (UINT_8 *) &dword;
    byte_sub(bytes, 4, true);
    return dword;
}

// 字节移位
UINT_32 rot_byte(UINT_32 dword) {
    auto *bytes = (UINT_8 *) &dword;
    auto *temp = new UINT_8[4];
    memcpy(temp, &dword, 4);
    bytes[0] = temp[1];
    bytes[1] = temp[2];
    bytes[2] = temp[3];
    bytes[3] = temp[0];
    delete[] temp;
    return dword;
}

// 拓展密钥
void extend_key(UINT_8 *key, UINT_32 *key_extend) {
    for (int i = 0; i < Nk; ++i) {
        memcpy(&key_extend[i], key + (4 * i), 4);
    }
    for (int i = Nk; i < Nb * (Nr + 1); ++i) {
        UINT_32 temp = key_extend[i - 1];
        if (i % Nk == 0) {
            temp = sub_byte(rot_byte(temp)) ^ turn_byte(R_CON[(i / Nk) - 1]);
        } else if (Nk > 6 && i % Nk == 4) {
            temp = sub_byte(temp);
        }
        key_extend[i] = key_extend[i - Nk] ^ temp;
    }
}

void handle_byte(UINT_8 *bytes, int len, UINT_8 *key, int key_len, bool isEncrypt) {
    Nb = handle_bytes_len / 4;
    Nk = key_len / 32;
    Nr = Nk_map_Nr[Nk];

    // 生成拓展密钥
    auto *key_extend_init = new UINT_32[Nb * (Nr + 1)];
    auto *key_extend_temp = new UINT_32[Nb * (Nr + 1)];
    extend_key(key, key_extend_init);

    for (int i = 0; i < len; i += handle_bytes_len) {
        // 现在开始处理 这handle_bytes_len个字节
        auto *now_handle = bytes + i;
        memcpy(key_extend_temp, key_extend_init, sizeof(UINT_32) * Nb * (Nr + 1));
        // 轮函数
        if (isEncrypt) {
            for (int j = 0; j < Nr + 1; ++j) {
                round_encrypt(now_handle, key_extend_temp + Nb * j, j);
            }
        } else {
            for (int j = 0; j < Nr + 1; ++j) {
                round_decrypt(now_handle, key_extend_temp + Nb * (Nr - j), Nr - j);
            }
        }

    }
    delete[] key_extend_init;
    delete[] key_extend_temp;
}

void encrypt_aes(UINT_8 *clear, UINT_8 *cipher, int len, UINT_8 *key, int key_len) {
    int new_len = (len + handle_bytes_len - 1) & ~(handle_bytes_len - 1);
    auto bytes = new UINT_8[new_len];
    memcpy(bytes, clear, len);
    memset(bytes + len, 0, new_len - len);
    handle_byte(bytes, new_len, key, key_len, true);
    memcpy(cipher, bytes, new_len);
    delete[] bytes;
}

void decrypt_aes(UINT_8 *cipher, UINT_8 *clear, int len, UINT_8 *key, int key_len) {
    int new_len = (len + handle_bytes_len - 1) & ~(handle_bytes_len - 1);
    auto bytes = new UINT_8[new_len];
    memcpy(bytes, cipher, len);
    memset(bytes + len, 0, new_len - len);
    handle_byte(bytes, new_len, key, key_len, false);
    memcpy(clear, bytes, new_len);
    delete[] bytes;
}

void handle_byte_cbc(UINT_8 *bytes, int len, UINT_8 *key, UINT_8 *iv, int key_len, bool isEncrypt) {
    Nb = handle_bytes_len / 4;
    Nk = key_len / 32;
    Nr = Nk_map_Nr[Nk];

    // 生成拓展密钥
    auto *key_extend_init = new UINT_32[Nb * (Nr + 1)];
    auto *key_extend_temp = new UINT_32[Nb * (Nr + 1)];
    extend_key(key, key_extend_init);

    // 加密
    if (isEncrypt) {
        // 拷贝iv
        auto temp = new UINT_8[handle_bytes_len];
        memcpy(temp, iv, handle_bytes_len);

        // 开始分组处理
        for (int i = 0; i < len; i += handle_bytes_len) {
            // 现在开始处理 这handle_bytes_len个字节
            auto *now_handle = bytes + i;
            // 拷贝密钥
            memcpy(key_extend_temp, key_extend_init, sizeof(UINT_32) * Nb * (Nr + 1));
            // 异或处理
            for (int j = 0; j < handle_bytes_len; j++) {
                now_handle[j] ^= temp[j];
            }
            // 轮函数
            for (int j = 0; j < Nr + 1; ++j) {
                round_encrypt(now_handle, key_extend_temp + Nb * j, j);
            }
            // 更新向量
            memcpy(temp, now_handle, handle_bytes_len);
        }
        delete[] temp;
    }

    // 解密
    if (!isEncrypt) {
        // 拷贝iv
        auto *temp1 = new UINT_8[handle_bytes_len];
        auto *temp2 = new UINT_8[handle_bytes_len];
        memcpy(temp2, iv, handle_bytes_len);

        // 开始分组处理
        for (int i = 0; i < len; i += handle_bytes_len) {
            // 现在开始处理 这handle_bytes_len个字节
            auto *now_handle = bytes + i;
            // 拷贝密钥
            memcpy(key_extend_temp, key_extend_init, sizeof(UINT_32) * Nb * (Nr + 1));
            // 保存上一组的密文
            memcpy(temp1, now_handle, handle_bytes_len);
            // 轮函数
            for (int j = 0; j < Nr + 1; ++j) {
                round_decrypt(now_handle, key_extend_temp + Nb * (Nr - j), Nr - j);
            }
            // 异或处理
            for (int j = 0; j < handle_bytes_len; j++) {
                now_handle[j] ^= temp2[j];
            }
            // 更新向量
            memcpy(temp2, temp1, handle_bytes_len);
        }
        delete[] temp1;
        delete[] temp2;
    }

    // 释放内存
    delete[] key_extend_init;
    delete[] key_extend_temp;
}

void encrypt_aes_cbc(const string &clear, string &cipher, UINT_8 *key, UINT_8 *iv, int key_len) {
    int len = (clear.length() + handle_bytes_len - 1) & ~(handle_bytes_len - 1);
    auto bytes = new UINT_8[len];
    copy(clear.begin(), clear.end(), bytes);
    memset(bytes + clear.length(), 0, len - clear.length());
    handle_byte_cbc(bytes, len, key, iv, key_len, true);
    cipher.resize(len);
    copy(bytes, bytes + len, cipher.begin());
    delete[] bytes;
}

void decrypt_aes_cbc(const string &cipher, string &clear, UINT_8 *key, UINT_8 *iv, int key_len) {
    int len = cipher.length();
    auto bytes = new UINT_8[len];
    copy(cipher.begin(), cipher.end(), bytes);
    handle_byte_cbc(bytes, len, key, iv, key_len, false);
    clear.resize(len);
    copy(bytes, bytes + len, clear.begin());
    // 去除填充0
    while (clear.back() == 0) {
        clear.pop_back();
    }
    delete[] bytes;
}

map<int, int> Nk_map_Nr{
        {4, 10},
        {6, 12},
        {8, 14},
};

const UINT_32 R_CON[] = {
        0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000,
        0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000,
        0xD8000000, 0xAB000000, 0x4D000000, 0x9A000000, 0x2F000000,
};

const UINT_32 S_Table[16][16] = {
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16,
};

const UINT_32 S_Table_Inv[16][16] = {
        0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
        0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
        0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
        0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
        0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
        0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
        0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
        0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
        0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
        0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
        0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
        0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
        0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
        0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
        0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
        0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d,
};

map<int, vector<int>> Nb_map_RowShiftNum{
        {4, {1, 2, 3}},
        {6, {1, 2, 3}},
        {8, {1, 3, 4}},
};

const UINT_8 ColMix_Table[4][4]{
        0x2, 0x3, 0x1, 0x1, 0x1, 0x2, 0x3, 0x1,
        0x1, 0x1, 0x2, 0x3, 0x3, 0x1, 0x1, 0x2,
};

const UINT_8 ColMix_Table_Inv[4][4]{
        0xE, 0xB, 0xD, 0x9, 0x9, 0xE, 0xB, 0xD,
        0xD, 0x9, 0xE, 0xB, 0xB, 0xD, 0x9, 0xE,
};