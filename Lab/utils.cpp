#include <bits/stdtr1c++.h>

using namespace std;

// 最大公约数
class BigCD {
public:
    static int get_BigCD(int x, int y) {
        if (y == 0) {
            return x;
        } else {
            return BigCD::get_BigCD(y, x % y);
        }
    }
};

// 素数生成
class Prime {
public:
    static bool is_prime(int num) {
        if (num <= 1) {
            return false;
        }
        int flag = 0;
        for (int i = 2; i < num; i++) {
            if (num % i == 0) flag++;
        }
        return flag == 0;
    }

    static int gcd(int x, int y) {
        if (y == 0)
            return x;
        else
            return gcd(y, x % y);
    }

    static bool is_relative_prime(int num1, int num2) {
        return gcd(num1, num2) == 1;
    }

    static vector<int> prime_set(int min_range, int max_range) {
        // 包含右边界
        ++max_range;
        // 素数列表
        vector<bool> temp_set{false, false, true, true};
        vector<int> prime_set{};
        temp_set.reserve(max_range);
        prime_set.reserve(max_range - min_range);
        // 先把每个数都定义为合数
        for (int i = 4; i < max_range; ++i) {
            temp_set.push_back(true);
        }
        // 令素数的倍数为false
        for (int i = 2; i < max_range; ++i) {
            if (temp_set[i]) {
                for (int j = i * 2; j < max_range; j += i) {
                    temp_set[j] = false;
                }
            }
        }
        // 保存在prime_set中
        for (int i = min_range; i < max_range; ++i) {
            if (temp_set[i]) {
                prime_set.push_back(i);
            }
        }
        //
        return prime_set;
    }
};

// 欧拉函数
class Euler {
public:
    static int get_Euler(int m, vector<int> &EdgeSet) {
        double euler = m;
        // 获得素数集
        vector<int> prime_set = Prime::prime_set(0, m);
        vector<int> index_set = {};
        vector<int> p_set = {};
        // 产生指数集
        for (auto num = prime_set.begin(); m != 1 && num != prime_set.end();
             ++num) {
            index_set.push_back(0);
            while (m % *num == 0) {
                ++(*(index_set.end() - 1));
                m /= *num;
            }
        }
        // 得到素数
        for (int i = 0; i < index_set.size(); ++i) {
            if (index_set[i] != 0) {
                p_set.push_back(prime_set[i]);
            }
        }
        // 得到欧拉函数
        for (int i: p_set) {
            EdgeSet.push_back(i);
            euler *= (1 - 1.0 / i);
        }
        return (int) euler;
    }
};

// 拓展欧几里得
class Euclid {
public:
    static int get_inverse(const int &r0, const int &r1, bool flag) {
        // 参数r0 > r1
        // 求r0逆元 flag = true
        // 求r1逆元 flag = false
        int r_last = r0, s_last = 1, t_last = 0;
        int r_next = r1, q_next, s_next = 0, t_next = 1;
        while (r_next != 0) {
            int r_temp = r_next, s_temp = s_next, t_temp = t_next;
            q_next = r_last / r_next;
            s_next = s_last - q_next * s_next;
            t_next = t_last - q_next * t_next;
            r_next = r_last - r_next * q_next;
            r_last = r_temp, s_last = s_temp, t_last = t_temp;
        }
        if (s_last < 0) {
            s_last += r1;
        }
        if (t_last < 0) {
            t_last += r0;
        }
        return (flag ? s_last : t_last);
    }
};

// 中国剩余定理
class RemainingTheorem {
public:
    static int getValue(const vector<pair<int, int>> &EdgeSet) {
        int m_mul = 1;
        vector<int> m;
        vector<int> M;
        vector<int> M_;
        unsigned len = EdgeSet.size();
        m.reserve(len);
        M.reserve(len);
        M_.reserve(len);
        // m
        for (int i = 0; i < len; ++i) {
            m.push_back(EdgeSet[i].first);
        }
        // M
        for (int i = 0; i < len; ++i) {
            int mul = 1;
            for (int j = 0; j < len; ++j) {
                if (j == i) {
                    continue;
                }
                mul *= m[j];
            }
            M.push_back(mul);
        }
        // M的逆元
        for (int i = 0; i < len; ++i) {
            M_.push_back(Euclid::get_inverse(M[i], m[i], true));
        }
        // m_mul
        for (int i = 0; i < len; ++i) {
            m_mul *= m[i];
        }
        // 解
        int x = 0;
        for (int i = 0; i < len; ++i) {
            x += M[i] * M_[i] * EdgeSet[i].second;
        }
        cout << "x = " << x << " = " << x % m_mul << " (mod " << m_mul << ")"
             << endl;
        return x % m_mul;
    }

    RemainingTheorem() {
        vector<pair<int, int>> EdgeSet{
                {43, 42},
                {59, 1},
        };
        cout << getValue(EdgeSet);
    }
};

string add(string num1, string num2) {
    // 只处理正数
    string num;
    unsigned len1 = num1.length();
    unsigned len2 = num2.length();
    if (len1 == 0 || len2 == 0) return "0";
    //前面补0，弄成长度相同
    if (len1 < len2) {
        for (int i = 1; i <= len2 - len1; ++i) {
            num1.insert(0, "0");
        }
    } else {
        for (int i = 1; i <= len1 - len2; ++i) {
            num2.insert(0, "0");
        }
    }
    //长度
    unsigned len = num1.length();
    unsigned temp1 = 0;  //进位
    unsigned temp2 = 0;  //各位的数字
    do {
        len--;
        temp2 = num1[len] - '0' + num2[len] - '0' + temp1;
        temp1 = temp2 / 10;  //判断是否进位
        temp2 %= 10;
        num.insert(0, to_string(temp2));
    } while (len != 0);
    if (temp1 != 0) num.insert(0, to_string(temp1));
    return num;
}

string sub(string num1, string num2) {
    // 只支持整数的相加减
    string num;
    unsigned len1 = num1.length();
    unsigned len2 = num2.length();
    if (len1 == 0 || len2 == 0) return "0";
    // 符号位
    string flag;
    //前面补0，弄成长度相同
    if (len1 < len2) {
        for (int i = 1; i <= len2 - len1; ++i) {
            num1.insert(0, "0");
        }
    } else {
        for (int i = 1; i <= len1 - len2; ++i) {
            num2.insert(0, "0");
        }
    }
    //长度
    unsigned len = num1.length();
    //经过处理 num1 > num2
    for (int i = 0; i < len; ++i) {
        if (num1[i] == num2[i]) {
            continue;
        } else if (num1[i] > num2[i]) {
            break;
        } else {
            string temp = num1;
            num1 = num2;
            num2 = temp;
            flag = '-';
            break;
        }
    }
    int temp1 = 0;  //借位
    int temp2 = 0;  //各位的数字
    do {
        len--;
        temp2 = num1[len] - num2[len] - temp1;  // 减去借位
        temp1 = 0;
        if (temp2 < 0) {
            temp1 = 1;
            temp2 += 10;
        }
        num.insert(0, to_string(temp2));
    } while (len != 0);
    //删除高位零
    num.erase(0, num.find_first_of("123456789"));
    num.insert(0, flag);
    return num;
}

// 单精度乘高精度
string mul(string num1, int num2) {
    string num = "0";
    string flag;
    unsigned len = num1.length();
    // 处理负号
    if (num1[0] == '-' && num2 > 0) {
        num1.erase(0, 1);
        --len;
        flag = "-";
    }
    if (num1[0] != '-' && num2 < 0) {
        num2 *= -1;
        flag = "-";
    }
    if (len == 0 || num2 == 0) {
        return "0";
    }
    int times = 0;
    do {
        unsigned len_temp = len;
        string temp;
        int temp1 = 0;  //进位
        int temp2 = 0;  //当前位的数字
        do {
            --len_temp;
            temp2 = ((num2 % 10)) * (num1[len_temp] - '0') + temp1;
            temp1 = temp2 / 10;
            temp2 %= 10;
            temp.insert(0, to_string(temp2));
        } while (len_temp != 0);
        // 有进位现象
        if (temp1 != 0) temp.insert(0, to_string(temp1));
        temp.append(times++, '0');  // 错位补0
        num = add(num, temp);
        num2 /= 10;
    } while (num2 != 0);
    num.insert(0, flag);
    return num;
}

string div(string num1, string num2, bool boolean) {
    /**
     * 参数说明
     * num1 被除数    要求 被除数除数大
     * num2 除数
     * boolean  true==>返回 商  false ==>返回 余数
     * */
    // 默认num1比num2长
    if (num2 == "0") return "除数为零";
    string num;
    string flag;
    unsigned len1 = num1.length();
    unsigned len2 = num2.length();
    // 处理负号
    if (num1[0] == '-' && num2[0] != '-') {
        num1.erase(0, 1);
        --len1;
        flag = "-";
    }
    if (num1[0] != '-' && num2[0] == '-') {
        num2.erase(0, 1);
        --len2;
        flag = "-";
    }
    if (len1 == 0 || len2 == 0) return "0";
    // 准备开始减
    unsigned time = len1 - len2;
    for (int i = 1; i <= time; ++i) {
        num2.insert(len2++, "0");
    }
    int temp;
    do {
        temp = 0;
        while (true) {
            // 判断谁大
            int flag2 = 0;
            if (num1.length() > num2.length()) {
                flag2 = 1;
            } else if (num1.length() < num2.length()) {
                flag2 = -1;
            } else {
                for (int i = 0; i < num1.length(); ++i) {
                    if (num1[i] == num2[i]) {
                        continue;
                    } else if (num1[i] > num2[i]) {
                        flag2 = 1;
                        break;
                    } else {
                        flag2 = -1;
                        break;
                    }
                }
            }
            if (flag2 == -1) {
                break;
            } else {
                num1 = sub(num1, num2);
                temp++;
            }
        }
        num.insert(num.length(), to_string(temp));
        num2.erase(--len2);
    } while (time--);
    //删除高位零
    num.erase(0, num.find_first_of("123456789"));
    num.insert(0, flag);  // 加上符号位
    if (boolean)
        return num;  // 返回商
    else
        return (num1.empty() ? "0" : num1);  // 返回余数
}

string high_precision_pow(int x, int y) {
    string str = "1";
    for (int i = 0; i < y; ++i) {
        str = mul(str, x);
    }
    return str;
}

// 原根
class Original_root {
public:
    static set<int> get_original_root(int m) {
        vector<int> EdgeSet;
        int f_m = Euler::get_Euler(m, EdgeSet);
        EdgeSet.clear();
        Euler::get_Euler(f_m, EdgeSet);
        if (EdgeSet.size() != 2) {
            return {};
        }
        int x1 = f_m / EdgeSet[0];
        int x2 = f_m / EdgeSet[1];
        int original_root = 0;
        for (int i = 2; i < f_m; ++i) {
            int mod = 0;
            if (x1 < 10) {
                mod = (int) pow(i, x1) % m;
            } else {
                mod = stoi(div(high_precision_pow(i, x1), to_string(m),
                               false));
            }
            if (mod < 0) {
                mod += m;
            }
            if (mod == 1) {
                continue;
            }
            if (x2 < 10) {
                mod = (int) pow(i, x2) % m;
            } else {
                mod = stoi(div(high_precision_pow(i, x2), to_string(m),
                               false));
            }
            if (mod < 0) {
                mod += m;
            }
            if (mod == 1) {
                continue;
            }
            original_root = i;
            break;
        }
        vector<int> relative_prime_set;
        for (int i = 1; i < f_m; ++i) {
            if (Prime::is_relative_prime(i, f_m)) {
                relative_prime_set.push_back(i);
            }
        }
        for (auto num: relative_prime_set) {
            cout << num << " ";
        }
        cout << endl;
        unsigned int len = relative_prime_set.size();
        set<int> original_root_set;
        for (int i = 0; i < len; ++i) {
            int num = 0;
            if (relative_prime_set[i] < 20) {
                num = (unsigned long long) pow(original_root,
                                               relative_prime_set[i]) %
                      m;
            } else {
                num = stoi(div(
                        high_precision_pow(original_root, relative_prime_set[i]),
                        to_string(m), false));
            }
            if (num < 0) {
                num += m;
            }
            original_root_set.insert(num);
        }
        return original_root_set;
    }
};

// 指数表
class Index_table {
    static int get_original_root(int m) {
        vector<int> EdgeSet;
        int f_m = Euler::get_Euler(m, EdgeSet);
        EdgeSet.clear();
        Euler::get_Euler(f_m, EdgeSet);
        if (EdgeSet.size() != 2) {
            return {};
        }
        int x1 = f_m / EdgeSet[0];
        int x2 = f_m / EdgeSet[1];
        int original_root = 0;
        for (int i = 2; i < f_m; ++i) {
            int mod = 0;
            if (x1 < 10) {
                mod = (int) pow(i, x1) % m;
            } else {
                mod = stoi(div(high_precision_pow(i, x1), to_string(m),
                               false));
            }
            if (mod < 0) {
                mod += m;
            }
            if (mod == 1) {
                continue;
            }
            if (x2 < 10) {
                mod = (int) pow(i, x2) % m;
            } else {
                mod = stoi(div(high_precision_pow(i, x2), to_string(m),
                               false));
            }
            if (mod < 0) {
                mod += m;
            }
            if (mod == 1) {
                continue;
            }
            original_root = i;
            break;
        }
        return original_root;
    }

public:
    static map<int, int> get_Index_table(int m) {
        map<int, int> index_map;
        int original_root = get_original_root(m);
        vector<int> EdgeSet;  // 没用
        int f_m = Euler::get_Euler(m, EdgeSet);
        for (int i = 0; i < f_m; ++i) {
            int num = 0;
            if (i < 20) {
                num = (unsigned long long) pow(original_root, i) % m;
            } else {
                num = stoi(div(high_precision_pow(original_root, i),
                               to_string(m), false));
            }
            index_map[num] = i;
        }
        return index_map;
    }
};
