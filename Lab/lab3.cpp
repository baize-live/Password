#include "password.h"

struct aes_test_case {
    int num, mode; // mode 1 = encrypt
    UINT_8 key[16], txt[16], out[16];
};

aes_test_case cases[] = {
        {1, 1,
                {0x00, 0x01, 0x20, 0x01, 0x71, 0x01, 0x98, 0xae, 0xda, 0x79, 0x17, 0x14, 0x60, 0x15, 0x35, 0x94},
                {0x00, 0x01, 0x00, 0x01, 0x01, 0xa1, 0x98, 0xaf, 0xda, 0x78, 0x17, 0x34, 0x86, 0x15, 0x35, 0x66},
                {0x6c, 0xdd, 0x59, 0x6b, 0x8f, 0x56, 0x42, 0xcb, 0xd2, 0x3b, 0x47, 0x98, 0x1a, 0x65, 0x42, 0x2a}
        },
        {2, 1,  {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c},
                {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34},
                {0x39, 0x25, 0x84, 0x1d, 0x02, 0xdc, 0x09, 0xfb, 0xdc, 0x11, 0x85, 0x97, 0x19, 0x6a, 0x0b, 0x32}
        },
};

int verifyCase() {
    UINT_8 result[16];
    for (auto &item: cases) {
        printf("case %d: \n", item.num);
        if (item.mode) {
            encrypt_aes(item.txt, result, 16, item.key, 128);
            printf("encrypt: ");
        } else {
            decrypt_aes(item.txt, result, 16, item.key, 128);
            printf("decrypt: ");
        }
        for (auto i: result) {
            printf("%02x ", i);
        }
        bool flag = true;
        for (int i = 0; i < sizeof(result); ++i) {
            if (result[i] != item.out[i]) {
                flag = false;
                break;
            }
        }
        printf("out: %d\n", flag);
    }
    return 0;
}

int detectionAvalanche() {
    UINT_8 bytes[16];
    UINT_8 bytes_1[16];
    UINT_8 bytes_2[16];
    int len = 0;
    encrypt_aes(cases[len].txt, bytes_1, 16, cases[len].key, 128);

    printf("====================================\n");

    // 固定密钥 修改明文中的一位
    int sum = 0;
    for (UINT_8 i = 0; i < 8; ++i) {
        memcpy(&bytes, cases[len].txt, 16);
        bytes[i] = bytes[i] ^ 0x80;
        // 加密
        encrypt_aes(bytes, bytes_2, 16, cases[len].key, 128);
        // 比较不同的位数
        int num = 0;
        for (int j = 0; j < sizeof(bytes); ++j) {
            int temp = bytes_1[j] ^ bytes_2[j];
            for (int k = 0; k < sizeof(UINT_8); ++k) {
                if ((temp >> 1) & 0x1) {
                    num++;
                }
            }
        }
        printf("%d. 固定密钥, 修改一位明文, 密文改变%d位\n", i + 1, num);
        sum += num;
    }
    printf("固定密钥, 修改一位明文, 密文平均改变%d位\n", sum / 8);

    printf("====================================\n");

    // 固定明文 修改密钥中的一位
    sum = 0;
    for (UINT_8 i = 0; i < 8; ++i) {
        memcpy(&bytes, cases[len].key, 8);
        bytes[i] = bytes[i] ^ 0x80;
        // 加密
        encrypt_aes(cases[len].txt, bytes_2, 16, bytes, 192);
        // 比较不同的位数
        int num = 0;
        for (int j = 0; j < sizeof(bytes); ++j) {
            int temp = bytes_1[j] ^ bytes_2[j];
            for (int k = 0; k < sizeof(UINT_8); ++k) {
                if ((temp >> 1) & 0x1) {
                    num++;
                }
            }
        }
        printf("%d. 固定明文, 修改一位密钥, 密文改变%d位\n", i + 1, num);
        sum += num;
    }
    printf("固定明文, 修改一位密钥, 密文平均改变%d位\n", sum / 8);

    return 0;
}

int main() {
    verifyCase();
    detectionAvalanche();
    system("pause");
}
