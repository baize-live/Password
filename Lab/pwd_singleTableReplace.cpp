#include <algorithm>
#include <vector>

#include "password.h"

map<char, char> encrypt_replace_table;
map<char, char> decrypt_replace_table;
vector<char> letter_frequency_table{
        'e', 't', 'o', 'i', 'a', 'n', 's', 'r', 'h', 'l', 'd', 'u', 'c',
        'm', 'p', 'y', 'f', 'g', 'w', 'b', 'v', 'k', 'x', 'j', 'q', 'z'
};

// 生成置换表(字符串)
void generate_replace_table(const string &key) {
    string word;
    for (char c: key) {
        if (isalpha(c) && word.find((char) tolower(c)) == string::npos) {
            word += (char) tolower(c);
        }
    }
    for (int i = 'a'; i <= 'z'; ++i) {
        if (word.find(char(i)) == string::npos) {
            word += char(i);
        }
    }
    for (int i = 'a'; i <= 'z'; i++) {
        encrypt_replace_table[i] = word[i - 'a'];
    }
    for (int i = 'A'; i <= 'Z'; i++) {
        encrypt_replace_table[i] = word[i - 'A'];
    }
    for (int i = 'a'; i <= 'z'; i++) {
        decrypt_replace_table[word[i - 'a']] = i;
    }
#ifdef debug
    printf("map<char, char> encrypt_replace_table = { \n");
    for (int i = 'a'; i <= 'z'; i++) {
        printf("{'%c', '%c'},\n", i, word[i - 'a']);
    }
    for (int i = 'A'; i <= 'Z'; i++) {
        printf("{'%c', '%c'},\n", i, word[i - 'A']);
    }
    printf("}; \n");

    printf("map<char, char> decrypt_replace_table = { \n");
    for (int i = 'a'; i <= 'z'; i++) {
        printf("{'%c', '%c'},\n", word[i - 'a'], i);
    }
    printf("}; \n");
#endif
}

string encrypt_singleTableReplace(const string &key, const string &clear) {
    generate_replace_table(key);

    string cipher{};
    for (const auto &c: clear) {
        if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')) {
            cipher += encrypt_replace_table[c];
            continue;
        }
        cipher += c;
    }
    return cipher;
}

string decrypt_singleTableReplace(const string &key, const string &cipher) {
    generate_replace_table(key);

    string clear{};
    for (const auto &c: cipher) {
        if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')) {
            clear += decrypt_replace_table[c];
            continue;
        }
        clear += c;
    }
    return clear;
}

// 频率攻击
string frequency_attack(const string &cipher) {
    map<int, int> frequency_map;
    int num = 0;
    // 初始化字母表
    for (int i = 'a'; i < 'z'; i++) {
        frequency_map[i] = 0;
    }
    // 统计字母频率
    for (const auto &c: cipher) {
        if (isalpha(c)) {
            ++num;
            frequency_map[tolower(c)]++;
        }
    }
    // 对频率排序
    vector<pair<int, int>> frequency_vec(
            frequency_map.begin(), frequency_map.end());
    sort(frequency_vec.begin(), frequency_vec.end(),
         [](pair<int, int> &a, pair<int, int> &b) { return a.second > b.second; });

    // 开始替换
    for (int i = 0; i < frequency_vec.size(); i++) {
        frequency_map[frequency_vec[i].first] = letter_frequency_table[i];
    }


    // 由频率确定
    frequency_map['s'] = 't';
    frequency_map['i'] = 'h';
    frequency_map['c'] = 'e';
    // sins ==> that
    frequency_map['n'] = 'a';
    // npc == > are
    frequency_map['p'] = 'r';
    // sm ==> to
    frequency_map['m'] = 'o';
    // sjmb ==> tion
    frequency_map['j'] = 'i';
    frequency_map['b'] = 'n';
    // qcrrnec ==> message
    frequency_map['q'] = 'm';
    // xmrrjhay ==> possibly
    frequency_map['a'] = 'l';
    // jbfmpqnsjmb ==> information
    frequency_map['f'] = 'f';
    // xmjbs ==> point
    frequency_map['x'] = 'p';
    // from a doint a to a doint i
    frequency_map['h'] = 'b';
    // bf means of
    frequency_map['y'] = 'y';
    // mentral ==> central
    frequency_map['g'] = 'c';
    // qcrrnec ==> message
    frequency_map['e'] = 'g';
    // pjeisfza ==> rightful
    frequency_map['z'] = 'u';
    // pcgmtcpcd ==> recovered
    frequency_map['d'] = 'd';
    frequency_map['t'] = 'v';
    // zbnzsimpjocd ==> unauthorized
    frequency_map['o'] = 'z';
    frequency_map['r'] = 's';
    // vim ==> who
    frequency_map['v'] = 'w';

    // 输出频率表
    for (const auto &pair: frequency_map) {
        printf("{ %c ==> %c } \n", pair.first, pair.second);
    }

    string clear = cipher;
    for (auto &c: clear) {
        c = (char) tolower(c);
        c = frequency_map[c];
    }

    return clear;
}
