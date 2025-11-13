#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(6000);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    connect(sock, (sockaddr*)&addr, sizeof(addr));
    cout << "Connected to RPC Server.\n";

    char func;
    int a, b;
    cout << "Choose operation (a)dd, (s)ub: ";
    cin >> func;
    cout << "Enter two integers: ";
    cin >> a >> b;

    write(sock, &func, sizeof(func));
    write(sock, &a, sizeof(a));
    write(sock, &b, sizeof(b));

    int res;
    read(sock, &res, sizeof(res));
    cout << "Result from server: " << res << endl;

    close(sock);
    return 0;
}
