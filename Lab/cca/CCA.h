#ifndef LAB_CCA_H
#define LAB_CCA_H
/*
大作业的要求：

1、作业需要先设计出保密通讯协议的过程和具体步骤；

2、分别编写A，B两个用户端的程序，写清楚两个程序分别要完成的功能，并能够在两个程序间进行通讯；

3、对AES加密的保密信息，要求采用CBC模式进行加密；

4、大作业的提交方式同实验报告的提交，也就是说既要提交程序实现的说明文档，也要提交源代码和可执行程序。

补充说明：本次大作业的最终成绩，将作为我们的期末成绩，也就是说，也就是说，这次大作业的战绩，将占本门课程成绩70%，在总成绩中占比非常高，故希望大家能足够重视，努力完成这次大作业！！！！
*/
// 保密通信协议
#include <ctime>
#include <thread>
#include <iostream>
#include <random>
#include "TCP.h"
#include "../password.h"
#include "../utils/FileUtil.h"

const short CCA_port = 2222;

random_device rd;
default_random_engine random(rd());
uniform_int_distribution<unsigned char> dis(0, 255);

string bytesToHexString(const BYTE *bytes, const int length) {
    if (bytes == nullptr) {
        return "";
    }
    std::string buf;
    const int len = length;
    for (int j = 0; j < len; j++) {
        int high = bytes[j] / 16, low = bytes[j] % 16;
        buf += (high < 10) ? ('0' + high) : ('a' + high - 10);
        buf += (low < 10) ? ('0' + low) : ('a' + low - 10);
    }
    return buf;
}

void hexStringToBytes(const std::string &hex, BYTE *bytes) {
    int byte_len = (int) hex.length() / 2;
    std::string strByte;
    unsigned int n;
    for (int i = 0; i < byte_len; i++) {
        strByte = hex.substr(i * 2, 2);
        sscanf(strByte.c_str(), "%x", &n);
        bytes[i] = n;
    }
}

string generate_random_string(int length) {
    string str;
    str.resize(length);

    for (int i = 0; i < length; i++) {
        str[i] = (char) dis(random);
    }
    return str;
}

class CCA {
protected:
    static int send(SOCKET &socket, const string &buf) {
        int len = (int) buf.size();
        ::send(socket, (char *) &len, sizeof(int), 0);
        TCP::send(socket, buf);
        return len;
    }

    static int recv(SOCKET &socket, string &buf) {
        int len;
        if (::recv(socket, (char *) &len, sizeof(int), 0) <= 0) {
            return -1;
        }
        buf.resize(len);
        TCP::recv(socket, buf);
        return len;
    }
};

class CCA_Server : CCA {
    // TODO: 现在只支持一对一
    pair<BigInt, BigInt> PKA;
    pair<BigInt, BigInt> SKA;
    pair<BigInt, BigInt> PKB;
    UINT_8 Aes_Key_Client[16]{};
    UINT_8 Aes_Key_Server[16]{};
    TCP_Server *server = nullptr;

    void read_CCA_Server_Key() {
        BigInt e, d, n;
        string user_path = getenv("USERPROFILE");
        string cca_path = ".cca";
        Dir dir(user_path + "\\" + cca_path);
        if (!dir.isExist()) {
            Dir::mkdir(user_path, cca_path);

            File file(user_path + "\\" + cca_path + "\\readme.txt");
            string connect = "This is a directory for CCA to save the key.";
            file.write(connect, ios::out);
        }

        string name = "server_key";
        File file(user_path + "\\" + cca_path + "\\" + name);

        string content;
        if (file.isExist()) {
            file.read(content);

            int begin = 0;
            int end;
            // e
            end = (int) content.find(' ', begin);
            if (end == string::npos) {
                goto newKey;
            }
            e = content.substr(begin, end - begin);
            begin = end + 1;
            // d
            end = (int) content.find(' ', begin);
            if (end == string::npos) {
                goto newKey;
            }
            d = content.substr(begin, end - begin);
            begin = end + 1;
            // n
            n = content.substr(begin);
            goto Key;
        }

        newKey:
        {
            printf("generate_key start...\n");
            generate_RSA_KEY(e, d, n);
            printf("generate_key success...\n");
            content = e.toString() + " " + d.toString() + " " + n.toString();
            file.write(content, ios::out);
        };

        Key:
        {
            PKA.first = e;
            PKA.second = n;
            SKA.first = d;
            SKA.second = n;
        };
    }

    // 发送公钥 PKA
    void send_PKA(SOCKET &client) const {
        string str = PKA.first.toString() + "\n" + PKA.second.toString();
        CCA::send(client, str);
    }

    // 接收公钥 PKB 和 AesKeyClient
    void recv_PKB_and_AesKeyClient(SOCKET &client) {
        string str;
        CCA::recv(client, str);

        // 解析公钥 PKB
        int begin = 0;
        int end;
        // PKB.first
        end = (int) str.find('\n', begin);
        PKB.first = str.substr(begin, end - begin);
        begin = end + 1;
        // PKB.second
        end = (int) str.find('\n', begin);
        PKB.second = str.substr(begin, end - begin);
        begin = end + 1;
        // 解密 AesKeyClient
        string AesKeyClient = str.substr(begin);
        string Key = decrypt_rsa(BigInt(AesKeyClient), SKA).toString();
        hexStringToBytes(Key, Aes_Key_Client);
    }

    // 发送 AesKeyServer
    void send_AesKeyServer(SOCKET &client) {
        string Key = generate_random_string(16);
        copy(Key.begin(), Key.end(), Aes_Key_Server);
        Key = bytesToHexString(Aes_Key_Server, 16);
        CCA::send(client, encrypt_rsa(BigInt(Key), PKB).toString());
    }

    // 处理客户端连接 进行密钥交换
    void handle_connect(SOCKET &client) {
        send_PKA(client);
        recv_PKB_and_AesKeyClient(client);
        send_AesKeyServer(client);
    }

public:
    explicit CCA_Server(bool &flag, short port = CCA_port) {
        // 读取密钥
        read_CCA_Server_Key();
        server = new TCP_Server(port, flag);
        if (!flag) {
            cout << "CCA_Server: TCP_Server init failed!" << endl;
        }
    }

    SOCKET accept() {
        SOCKET client = server->tcp_accept();
        handle_connect(client);

        cout << "CCA_Server connect success!" << endl;

        return client;
    }

    void send(SOCKET &client, const string &clear) {
        string iv = generate_random_string(16);
        string cipher;
        // 加密
        encrypt_aes_cbc(clear, cipher, Aes_Key_Server, (UINT_8 *) iv.c_str(), 128);
        // 发送 iv
        CCA::send(client, iv);
        // 发送 cipher
        CCA::send(client, cipher);
    }

    void recv(SOCKET &client, string &clear) {
        string iv;
        string cipher;
        // 接收 iv
        if (CCA::recv(client, iv) < 0) {
            clear.clear();
            cout << "network error" << endl;
            exit(-1);
        }
        // 接收 cipher
        CCA::recv(client, cipher);
        // 解密
        decrypt_aes_cbc(cipher, clear, Aes_Key_Client, (UINT_8 *) iv.c_str(), 128);
    }

    void close() {
        server->tcp_close();
        delete server;
    }
};

class CCA_Client : CCA {
    pair<BigInt, BigInt> PKB;
    pair<BigInt, BigInt> SKB;
    pair<BigInt, BigInt> PKA;
    UINT_8 Aes_Key_Client[16]{};
    UINT_8 Aes_Key_Server[16]{};
    SOCKET server{};
    TCP_Client *client = nullptr;

    void read_CCA_Client_Key() {
        BigInt e, d, n;
        string user_path = getenv("USERPROFILE");
        string cca_path = ".cca";
        Dir dir(user_path + "\\" + cca_path);
        if (!dir.isExist()) {
            Dir::mkdir(user_path, cca_path);

            File file(user_path + "\\" + cca_path + "\\readme.txt");
            string connect = "This is a directory for CCA to save the key.";
            file.write(connect, ios::out);
        }

        string name = "client_key";
        File file(user_path + "\\" + cca_path + "\\" + name);

        string content;
        if (file.isExist()) {
            file.read(content);
            int begin = 0;
            int end;
            // e
            end = (int) content.find(' ', begin);
            if (end == string::npos) {
                goto newKey;
            }
            e = content.substr(begin, end - begin);
            begin = end + 1;
            // d
            end = (int) content.find(' ', begin);
            if (end == string::npos) {
                goto newKey;
            }
            d = content.substr(begin, end - begin);
            begin = end + 1;
            // n
            n = content.substr(begin);
            goto Key;
        }

        newKey:
        {
            printf("generate_key start...\n");
            generate_RSA_KEY(e, d, n);
            printf("generate_key success...\n");
            content = e.toString() + " " + d.toString() + " " + n.toString();
            file.write(content, ios::out);
        };

        Key:
        {
            PKB.first = e;
            PKB.second = n;
            SKB.first = d;
            SKB.second = n;
        };
    }

    // 接收公钥 PKA
    void recv_PKA() {
        string str;
        CCA::recv(server, str);
        string temp;
        // 解析公钥 PKA
        int end = (int) str.find('\n');
        PKA.first = str.substr(0, end);
        PKA.second = str.substr(end + 1);
    }

    // 发送公钥 PKB 和 AesKeyClient
    void send_PKB_and_AesKeyClient() {
        string Key = generate_random_string(16);
        copy(Key.begin(), Key.end(), Aes_Key_Client);
        Key = bytesToHexString(Aes_Key_Client, 16);

        string AesKeyClient = encrypt_rsa(Key, PKA).toString();
        string str = PKB.first.toString() + "\n" + PKB.second.toString() + "\n" + AesKeyClient;
        CCA::send(server, str);
    }

    // 接收 AesKeyServer
    void recv_AesKeyServer() {
        string str;
        CCA::recv(server, str);
        string key = decrypt_rsa(BigInt(str), SKB).toString();
        hexStringToBytes(key, Aes_Key_Server);
    }

    // 与服务器建立连接 进行密钥交换
    void handle_connect() {
        recv_PKA();
        send_PKB_and_AesKeyClient();
        recv_AesKeyServer();
    }

public:
    CCA_Client() {
        // 读取密钥
        read_CCA_Client_Key();
    }

    void connect(bool &flag, const string &host, short port = CCA_port) {
        client = new TCP_Client(flag);

        if (!flag) {
            printf("CCA_Client: TCP_Client init failed!\n");
            return;
        }

        flag = false;
        server = client->tcp_connect(host, port, flag);

        if (!flag) {
            printf("CCA_Client: TCP_Client connect failed!\n");
            return;
        }

        handle_connect();

        cout << "CCA_Client connect success! " << endl;
    }

    void send(const string &clear) {
        string iv = generate_random_string(16);
        string cipher;
        // 加密
        encrypt_aes_cbc(clear, cipher, Aes_Key_Client, (UINT_8 *) iv.c_str(), 128);
        // 发送 iv
        CCA::send(server, iv);
        // 发送 cipher
        CCA::send(server, cipher);
    }

    void recv(string &clear) {
        string iv;
        string cipher;
        // 接收 iv
        if (CCA::recv(server, iv) < 0) {
            clear.clear();
            cout << "network error" << endl;
            exit(-1);
        }
        // 接收 cipher
        CCA::recv(server, cipher);
        // 解密
        decrypt_aes_cbc(cipher, clear, Aes_Key_Server, (UINT_8 *) iv.c_str(), 128);
    }

    void close() {
        client->tcp_close();
        delete client;
    }
};

#endif //LAB_CCA_H
