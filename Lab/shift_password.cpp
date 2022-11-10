#include "password.h"

using namespace std;

void encrypt_shift(byte *clear, int len, int shift_num) {
    shift_num %= 26;
    for (int i = 0; i < len; i++) {
        if ('a' <= clear[i] && clear[i] <= 'z') {
            byte temp = (clear[i] + shift_num) % ('z' + 1);
            clear[i] = (temp < 26) ? temp + 'a' : temp;
        } else if ('A' <= clear[i] && clear[i] <= 'Z') {
            byte temp = (clear[i] + shift_num) % ('Z' + 1);
            temp = (temp < 26) ? temp + 'a' : temp;
            clear[i] = (temp < 'a') ? temp + 32 : temp;
        }
    }
}

void decrypt_shift(byte *cipher, int len, int shift_num) {
    shift_num %= 26;
    for (int i = 0; i < len; i++) {
        if ('a' <= cipher[i] && cipher[i] <= 'z') {
            byte temp = (cipher[i] - shift_num) % ('z' + 1);
            cipher[i] = temp < 'a' ? temp + 26 : temp;
        } else if ('A' <= cipher[i] && cipher[i] <= 'Z') {
            byte temp = (cipher[i] - shift_num) % ('Z' + 1);
            temp = temp < 'A' ? temp + 26 : temp;
            cipher[i] = temp < 'a' ? temp + 32 : temp;
        }
    }
}

string encrypt_shift(const string &clear, int shift_num) {
    byte *bytes = new byte[clear.size()];
    for (int i = 0; i < clear.size(); i++) bytes[i] = clear[i];
    encrypt_shift(bytes, (int) clear.size(), shift_num);
    string cipher;
    cipher.resize(clear.size());
    copy(bytes, bytes + clear.size(), cipher.begin());
    delete[] bytes;
    return cipher;
}

string decrypt_shift(const string &cipher, int shift_num) {
    byte *bytes = new byte[cipher.size()];
    for (int i = 0; i < cipher.size(); i++) bytes[i] = cipher[i];
    decrypt_shift(bytes, (int) cipher.size(), shift_num);
    string clear;
    clear.resize(cipher.size());
    copy(bytes, bytes + cipher.size(), clear.begin());
    delete[] bytes;
    return clear;
}

void attack_shift(const string &cipher) {
    byte *bytes = new byte[cipher.size()];
    printf("start attack shift password\n");
    for (int shift_num = 0; shift_num < 26; ++shift_num) {
        printf("shift: %02d, decrypt result: ", shift_num);
        for (int i = 0; i < cipher.size(); i++) {
            bytes[i] = cipher[i];
        }
        decrypt_shift(bytes, (int) cipher.size(), shift_num);
        for (int i = 0; i < cipher.size(); i++) {
            printf("%c", bytes[i]);
        }
        printf("\n");
    }
    delete[] bytes;
}