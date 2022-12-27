#ifndef LAB1_PASSWORD_H
#define LAB1_PASSWORD_H

#include <string>
#include <vector>
#include <cstring>
#include <map>
#include "BigInt.h"

using namespace std;

typedef unsigned char UINT_8;
typedef unsigned int UINT_32;
typedef unsigned long long int UINT_64;

// 移位密码
string encrypt_shift(const string &clear, int shift_num);

string decrypt_shift(const string &cipher, int shift_num);

void attack_shift(const string &cipher);

// 单表置换密码
// 生成置换表
void generate_replace_table(const string &key = "For you, a thousand times over. zzz");

string encrypt_singleTableReplace(const string &key, const string &clear);

string decrypt_singleTableReplace(const string &key, const string &cipher);

string frequency_attack(const string &cipher);

// DES密码
void encrypt_des(UINT_8 *clear, UINT_8 *cipher, int len, UINT_64 key_64);

void decrypt_des(UINT_8 *cipher, UINT_8 *clear, int len, UINT_64 key_64);

// AES密码 密钥长度可以为 128 192 256
void encrypt_aes(UINT_8 *clear, UINT_8 *cipher, int len, UINT_8 *key, int key_len);

void decrypt_aes(UINT_8 *cipher, UINT_8 *clear, int len, UINT_8 *key, int key_len);

// RSA密码
void generate_Key();

BigInt encrypt_rsa(const BigInt &clear, const pair<BigInt, BigInt> &pub_key);

BigInt decrypt_rsa(const BigInt &cipher, const pair<BigInt, BigInt> &pri_key);

// MD5 hash
void MD5(UINT_8 *clear, UINT_8 *cipher, int len);

#endif //LAB1_PASSWORD_H
