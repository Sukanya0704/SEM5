#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function that will run for each client
void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = read(client_socket, buffer, BUFFER_SIZE - 1);

        if (bytes_received <= 0) {
            std::cout << "Client disconnected.\n";
            close(client_socket);
            break;
        }

        std::cout << "Client says: " << buffer;

        // Send the same message back
        send(client_socket, buffer, strlen(buffer), 0);
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    // 1. Create a TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Setup server address (IP + Port)
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP
    address.sin_port = htons(PORT);

    // 3. Bind socket to the address
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));

    // 4. Start listening (max 5 waiting clients)
    listen(server_fd, 5);

    std::cout << "Server started. Listening on port " << PORT << "...\n";

    // 5. Accept clients continuously
    while (true) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        std::cout << "New client connected.\n";

        // Create a thread to handle this client
        std::thread client_thread(handle_client, new_socket);
        client_thread.detach(); // Run thread independently
    }

    close(server_fd);
    return 0;
}