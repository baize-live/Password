#ifndef LAB1_PASSWORD_H
#define LAB1_PASSWORD_H

#include <string>

typedef unsigned char byte;

// “∆Œª√‹¬Î
void encrypt_shift(byte *clear, int len, int shift_num);

void decrypt_shift(byte *cipher, int len, int shift_num);

std::string encrypt_shift(const std::string &clear, int shift_num);

std::string decrypt_shift(const std::string &cipher, int shift_num);

void attack_shift(const std::string &cipher);

#endif //LAB1_PASSWORD_H
