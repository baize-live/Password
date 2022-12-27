#include "password.h"

//将x循环左移n位
#define CYC_SHL_32(x, n) (((x) << (n)) | ((x) >> (32-(n))))

//非线性函数,每组四轮,每轮使用依次一个
#define F(x, y, z) (((x) & (y)) | ((~(x)) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~(z))))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~(z))))

#define FF(a, b, c, d, Mj, s, ti) ((a) = (b) + (CYC_SHL_32(((a) + F((b),(c),(d)) + (Mj) + (ti)), (s))))
#define GG(a, b, c, d, Mj, s, ti) ((a) = (b) + (CYC_SHL_32(((a) + G((b),(c),(d)) + (Mj) + (ti)), (s))))
#define HH(a, b, c, d, Mj, s, ti) ((a) = (b) + (CYC_SHL_32(((a) + H((b),(c),(d)) + (Mj) + (ti)), (s))))
#define II(a, b, c, d, Mj, s, ti) ((a) = (b) + (CYC_SHL_32(((a) + I((b),(c),(d)) + (Mj) + (ti)), (s))))

//第1/4轮的循环左移次数,循环使用
#define FS1 7
#define FS2 12
#define FS3 17
#define FS4 22
//第2/4轮的循环左移次数,循环使用
#define GS1 5
#define GS2 9
#define GS3 14
#define GS4 20
//第3/4轮的循环左移次数,循环使用
#define HS1 4
#define HS2 11
#define HS3 16
#define HS4 23
//第4/4轮的循环左移次数,循环使用
#define IS1 6
#define IS2 10
#define IS3 15
#define IS4 21

//链接变量
UINT_32 linkA;
UINT_32 linkB;
UINT_32 linkC;
UINT_32 linkD;

// 处理每个分组
void handle_group(const UINT_32 *M) {
    //临时链接变量
    UINT_32 a = linkA, b = linkB, c = linkC, d = linkD;

    //第1/4轮,含有16步
    FF(a, b, c, d, M[0], FS1, 0xd76aa478);
    FF(d, a, b, c, M[1], FS2, 0xe8c7b756);
    FF(c, d, a, b, M[2], FS3, 0x242070db);
    FF(b, c, d, a, M[3], FS4, 0xc1bdceee);

    FF(a, b, c, d, M[4], FS1, 0xf57c0faf);
    FF(d, a, b, c, M[5], FS2, 0x4787c62a);
    FF(c, d, a, b, M[6], FS3, 0xa8304613);
    FF(b, c, d, a, M[7], FS4, 0xfd469501);

    FF(a, b, c, d, M[8], FS1, 0x698098d8);
    FF(d, a, b, c, M[9], FS2, 0x8b44f7af);
    FF(c, d, a, b, M[10], FS3, 0xffff5bb1);
    FF(b, c, d, a, M[11], FS4, 0x895cd7be);

    FF(a, b, c, d, M[12], FS1, 0x6b901122);
    FF(d, a, b, c, M[13], FS2, 0xfd987193);
    FF(c, d, a, b, M[14], FS3, 0xa679438e);
    FF(b, c, d, a, M[15], FS4, 0x49b40821);

    //第2/4轮,含有16步
    GG(a, b, c, d, M[1], GS1, 0xf61e2562);
    GG(d, a, b, c, M[6], GS2, 0xc040b340);
    GG(c, d, a, b, M[11], GS3, 0x265e5a51);
    GG(b, c, d, a, M[0], GS4, 0xe9b6c7aa);

    GG(a, b, c, d, M[5], GS1, 0xd62f105d);
    GG(d, a, b, c, M[10], GS2, 0x02441453);
    GG(c, d, a, b, M[15], GS3, 0xd8a1e681);
    GG(b, c, d, a, M[4], GS4, 0xe7d3fbc8);

    GG(a, b, c, d, M[9], GS1, 0x21e1cde6);
    GG(d, a, b, c, M[14], GS2, 0xc33707d6);
    GG(c, d, a, b, M[3], GS3, 0xf4d50d87);
    GG(b, c, d, a, M[8], GS4, 0x455a14ed);

    GG(a, b, c, d, M[13], GS1, 0xa9e3e905);
    GG(d, a, b, c, M[2], GS2, 0xfcefa3f8);
    GG(c, d, a, b, M[7], GS3, 0x676f02d9);
    GG(b, c, d, a, M[12], GS4, 0x8d2a4c8a);

    //第3/4轮,含有16步
    HH(a, b, c, d, M[5], HS1, 0xfffa3942);
    HH(d, a, b, c, M[8], HS2, 0x8771f681);
    HH(c, d, a, b, M[11], HS3, 0x6d9d6122);
    HH(b, c, d, a, M[14], HS4, 0xfde5380c);

    HH(a, b, c, d, M[1], HS1, 0xa4beea44);
    HH(d, a, b, c, M[4], HS2, 0x4bdecfa9);
    HH(c, d, a, b, M[7], HS3, 0xf6bb4b60);
    HH(b, c, d, a, M[10], HS4, 0xbebfbc70);

    HH(a, b, c, d, M[13], HS1, 0x289b7ec6);
    HH(d, a, b, c, M[0], HS2, 0xeaa127fa);
    HH(c, d, a, b, M[3], HS3, 0xd4ef3085);
    HH(b, c, d, a, M[6], HS4, 0x04881d05);

    HH(a, b, c, d, M[9], HS1, 0xd9d4d039);
    HH(d, a, b, c, M[12], HS2, 0xe6db99e5);
    HH(c, d, a, b, M[15], HS3, 0x1fa27cf8);
    HH(b, c, d, a, M[2], HS4, 0xc4ac5665);

    //第4/4轮,含有16步
    II(a, b, c, d, M[0], IS1, 0xf4292244);
    II(d, a, b, c, M[7], IS2, 0x432aff97);
    II(c, d, a, b, M[14], IS3, 0xab9423a7);
    II(b, c, d, a, M[5], IS4, 0xfc93a039);

    II(a, b, c, d, M[12], IS1, 0x655b59c3);
    II(d, a, b, c, M[3], IS2, 0x8f0ccc92);
    II(c, d, a, b, M[10], IS3, 0xffeff47d);
    II(b, c, d, a, M[1], IS4, 0x85845dd1);

    II(a, b, c, d, M[8], IS1, 0x6fa87e4f);
    II(d, a, b, c, M[15], IS2, 0xfe2ce6e0);
    II(c, d, a, b, M[6], IS3, 0xa3014314);
    II(b, c, d, a, M[13], IS4, 0x4e0811a1);

    II(a, b, c, d, M[4], IS1, 0xf7537e82);
    II(d, a, b, c, M[11], IS2, 0xbd3af235);
    II(c, d, a, b, M[2], IS3, 0x2ad7d2bb);
    II(b, c, d, a, M[9], IS4, 0xeb86d391);

    linkA += a;
    linkB += b;
    linkC += c;
    linkD += d;
}

// MD5算法
void MD5(UINT_8 *clear, UINT_8 *hash, int len) {
    // 不含填充信息的分组数量
    int group_num = len >> 6;

    // 初始化
    linkA = 0x67452301;
    linkB = 0xefcdab89;
    linkC = 0x98badcfe;
    linkD = 0x10325476;

    // 处理不含填充信息的分组,共有group_num个
    for (int i = 0; i < group_num; ++i) {
        handle_group((UINT_32 *) (clear + (i << 6)));
    }

    // 填充字节数
    int fill_byte_num = len & 63;
    UINT_8 group_byte[64];

    // 处理含填充信息的分组
    if (fill_byte_num < 56) {
        // 填充信息只有一组
        memset(group_byte, 0, 56);
        memcpy(group_byte, clear + (group_num << 6), fill_byte_num);
        group_byte[fill_byte_num] = 0x80;
        // 扩展,原信息的bit数量,64位.
        UINT_64 bit_len = len << 3;
        memcpy(group_byte + 56, &bit_len, 8);
        // 处理分组
        handle_group((UINT_32 *) group_byte);
    } else {
        // 填充信息有两组
        // 第一个分组
        memset(group_byte, 0, 64);
        memcpy(group_byte, clear + (group_num << 6), fill_byte_num);
        group_byte[fill_byte_num] = 0x80;
        // 处理分组
        handle_group((UINT_32 *) group_byte);
        // 第二个分组
        memset(group_byte, 0, 56);
        UINT_64 bit_len = len << 3;
        memcpy(group_byte + 56, &bit_len, 8);
        // 处理分组
        handle_group((UINT_32 *) group_byte);
    }

    // 取得最终MD5的hash
    memcpy(hash, &linkA, 4);
    memcpy(hash + 4, &linkB, 4);
    memcpy(hash + 8, &linkC, 4);
    memcpy(hash + 12, &linkD, 4);
}
