#ifndef LAB1_PASSWORD_H
#define LAB1_PASSWORD_H

#include <string>
#include <Cstring>
#include <map>

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

#endif //LAB1_PASSWORD_H
