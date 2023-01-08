#ifndef LAB_TCP_H
#define LAB_TCP_H

#include <string>
#include "winsock2.h"
#include <vector>

using namespace std;

const int buf_len = 2048;

class TCP {
public:
    static void recv(SOCKET client, string &in_buf) {
        auto buf = new char[in_buf.size()];
        int len = ::recv(client, buf, (int) in_buf.size(), 0);
        in_buf.resize(len);
        copy(buf, buf + len, in_buf.begin());
        delete[] buf;
    }

    static void recv(SOCKET client, vector<char> &in_buf) {
        auto buf = new char[in_buf.size()];
        int len = ::recv(client, buf, (int) in_buf.size(), 0);
        in_buf.resize(len);
        copy(buf, buf + len, in_buf.begin());
        delete[] buf;
    }

    static void send(SOCKET client, const string &out_buf) {
        char buf[buf_len];
        int len = (int) out_buf.size();
        int pos = 0;
        while (pos < len) {
            int send_len = min(len - pos, buf_len);
            copy(out_buf.begin() + pos, out_buf.begin() + pos + send_len, buf);
            ::send(client, buf, send_len, 0);
            pos += send_len;
        }
    }

    static void send(SOCKET client, const vector<char> &out_buf) {
        char buf[buf_len];
        int len = (int) out_buf.size();
        int pos = 0;
        while (pos < len) {
            int send_len = min(len - pos, buf_len);
            copy(out_buf.begin() + pos, out_buf.begin() + pos + send_len, buf);
            ::send(client, buf, send_len, 0);
            pos += send_len;
        }
    }
};

class TCP_Server {
private:
    SOCKET server;

public:
    explicit TCP_Server(short port, bool &flag) {
        WSADATA wsd{};
        // ��ʼ�� socket���� //��һ��������dll�汾�ţ��ڶ��������Ǵ�����socket���� ��Ҫȷ���Ƿ��ʼ���ɹ�
        if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
            return;
        }
        // ����socket
        server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server == INVALID_SOCKET) {
            return;
        }

        // �����׽��ֵ�ַ
        sockaddr_in server_addr{};
        server_addr.sin_family = PF_INET;
        server_addr.sin_port = htons(port);//�˿�
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//����ĵ�ַ
        if (bind(server, (sockaddr *) &server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            return;
        }

        // ��ʼ����
        if (listen(server, 10) == SOCKET_ERROR) {
            return;
        }
        flag = true;
    }

    // ����һ���ͻ�������
    SOCKET tcp_accept() const {
        sockaddr_in client_sin{};
        int len = sizeof(client_sin);
        SOCKET client = accept(server, (sockaddr *) &client_sin, &len);
        return client;
    }

    void tcp_close() const {
        closesocket(server);
        WSACleanup();
    }
};

class TCP_Client {
private:
    SOCKET client;

public:
    explicit TCP_Client(bool &flag) {
        WSADATA wsd{};
        // ��ʼ�� socket���� //��һ��������dll�汾�ţ��ڶ��������Ǵ�����socket���� ��Ҫȷ���Ƿ��ʼ���ɹ�
        if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
            return;
        }
        // ����socket����
        client = socket(AF_INET, SOCK_STREAM, 0);
        flag = true;
    }

    SOCKET tcp_connect(const string &host, const short port, bool &flag) const {
        // ���÷�������Ϣ
        sockaddr_in client_addr{};
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(port);
        char *host_temp = new char[host.size() + 1];
        copy(host.cbegin(), host.cend(), host_temp);
        host_temp[host.size()] = '\0';

        client_addr.sin_addr.S_un.S_addr = inet_addr(host_temp);
        if (connect(client, (sockaddr *) &client_addr, sizeof(client_addr)) == 0) {
            flag = true;
        }
        delete[] host_temp;
        return client;
    }

    void tcp_close() const {
        closesocket(client);
        WSACleanup();
    }
};

#endif //LAB_TCP_H
