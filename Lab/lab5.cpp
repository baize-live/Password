#include "password.h"

struct md5_test_case {
    string msg;
    UINT_8 hash[16];
};
md5_test_case cases[] = {
        {"",    {0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e}},
        {"a",   {0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8, 0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61}},
        {"abc", {0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0, 0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72}},
        {"message digest",
                {0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d, 0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0}},
        {"abcdefghijklmnopqrstuvwxyz",
                {0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00, 0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b}},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
                {0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5, 0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f}},
        {"12345678901234567890123456789012345678901234567890123456789012345678901234567890",
                {0x57, 0xed, 0xf4, 0xa2, 0x2b, 0xe3, 0xc9, 0x55, 0xac, 0x49, 0xda, 0x2e, 0x21, 0x07, 0xb6, 0x7a}},

};

int verifyCase() {
    for (const md5_test_case &caseItem: cases) {
        UINT_8 hash[16];
        MD5((UINT_8 *) caseItem.msg.c_str(), hash, (int) caseItem.msg.size());
        printf("hash: ");
        for (auto i: hash) {
            printf("%02x ", i);
        }
        bool flag = true;
        for (int i = 0; i < sizeof(hash); ++i) {
            if (hash[i] != caseItem.hash[i]) {
                flag = false;
                break;
            }
        }
        printf("out: %d\n", flag);
    }
    return 0;
}

int detectionAvalanche() {
    string clear = "I love three things in the world the sun the moon and you The sun for the day The moon for the night and you forever ";
    UINT_8 hash[16];
    MD5((UINT_8 *) clear.c_str(), hash, (int) clear.size());

    // 固定密钥 修改明文中的一位
    int sum = 0;
    UINT_8 new_hash[16];
    for (int i = 0; i < 8; ++i) {
        // 修改1位
        clear[i * clear.size() / 8] = ' ';
        // hash
        MD5((UINT_8 *) clear.c_str(), new_hash, (int) clear.size());
        // 比较不同的位数
        int num = 0;
        for (int j = 0; j < 16; ++j) {
            int temp = hash[j] ^ new_hash[j];
            for (int k = 0; k < sizeof(UINT_8); ++k) {
                if ((temp >> 1) & 0x1) {
                    num++;
                }
            }
        }
        printf("%d. 修改一位字符, hash值改变%d位\n", i + 1, num);
        sum += num;
    }
    printf("修改一位字符, hash值平均改变%d位\n", sum / 8);
    return 0;
}

int main() {
    verifyCase();
    detectionAvalanche();
    system("pause");
}