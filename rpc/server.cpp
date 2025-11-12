#include <bits/stdc++.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
using namespace std;

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }

void handleClient(int clientSocket) {
    char func;
    int a, b;
    read(clientSocket, &func, sizeof(func));
    read(clientSocket, &a, sizeof(a));
    read(clientSocket, &b, sizeof(b));

    int res = 0;
    if (func == 'a')
        res = add(a, b);
    else if (func == 's')
        res = sub(a, b);

    cout << "Result " << res << " sent to client.\n";
    write(clientSocket, &res, sizeof(res));
    close(clientSocket);
}

int main() {
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(6000);

    bind(serverFd, (sockaddr*)&addr, sizeof(addr));
    listen(serverFd, 5);

    cout << "🛰️ RPC Server running on port 6000...\n";

    while (true) {
        int clientSocket = accept(serverFd, NULL, NULL);
        cout << "💡 New RPC client connected.\n";
        thread(handleClient, clientSocket).detach();
    }

    close(serverFd);
    return 0;
}
 server.cpp