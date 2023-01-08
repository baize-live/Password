#include "CCA.h"

int main() {
    CCA_Client client;
    bool flag = false;
    client.connect(flag, "127.0.0.1");
    if (!flag) {
        printf("CCA_Client: connect failed!\n");
        return 0;
    }


    string dir;
    client.recv(dir);

    string str;
    while (true) {
        cout << dir << "> ";
        getline(cin, str);

        if (str == "exit") {
            client.send(str);
            break;
        }
        if (str == "clear" || str == "cls") {
            system("cls");
            continue;
        }
        if (str == "ls") {
            str = "dir";
        }
        client.send(str);
        client.recv(str);
        cout << str << endl;
    }
    client.close();
    return 0;
}