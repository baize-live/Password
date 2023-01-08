#include "CCA.h"

void executeCMD(string &cmd, string &res) {
    FILE *fp;
    cmd += " 2>&1";
    if ((fp = _popen(cmd.c_str(), "r")) != nullptr) {
        res.clear();
        char buf[512];
        while (fgets(buf, sizeof(buf), fp)) {
            res.append(buf);
        }

        if (res.empty()) {
            res = " ";
        }
    }
    if (res.empty()) {
        res = "error ";
    }
    pclose(fp);
}

void handle_connect(CCA_Server *server, SOCKET *client) {
    // 发送当前路径
    char buf[512];
    _getcwd(buf, 512);
    string str(buf);
    server->send(*client, str);

    string cmd;
    while (true) {
        server->recv(*client, cmd);

        if (cmd.empty() || cmd == "exit") {
            break;
        }
        cout << "cmd: " << cmd << endl;
        executeCMD(cmd, cmd);
        server->send(*client, cmd);
    }
}

int main() {
    bool flag = false;
    CCA_Server server(flag);
    if (!flag) {
        printf("CCA_Server: init failed!\n");
        return 0;
    }

    printf("CCA_Server start success \n");

    while (true) {
        SOCKET client = server.accept();
        thread handle(&handle_connect, &server, &client);
        handle.join();
    }

}