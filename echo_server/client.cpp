#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet>
using namespace std;

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    // 1. Create a TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Set server address (localhost:8080)
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address from text to binary (127.0.0.1 → localhost)
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // 3. Connect to the server
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    cout << "Connected to server. Type messages:\n";

    // 4. Send and receive messages
    while (true) {
        string msg;
        cout << "> ";
        getline(cin, msg);

        // Send message
        send(sock, msg.c_str(), msg.size(), 0);

        // Receive echo
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = read(sock, buffer, BUFFER_SIZE - 1);

        if (bytes_received <= 0) {
            cout << "Server disconnected.\n";
            break;
        }

        cout << "Echo from server: " << buffer << "\n";
    }

    close(sock);
    return 0;
}