#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 1236
#define BUFFER_SIZE 1024

int main() {
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    int server_socket, client_socket, addr_len;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all network interfaces
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    listen(server_socket, 5);

    printf("Server Established\n");
    printf("Listening to 0.0.0.0:%d\n", SERVER_PORT);

    while (1) {
        addr_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t*)&addr_len);

        if (client_socket < 0) {
            perror("Acceptance failed");
            exit(EXIT_FAILURE);
        }

        while (recv(client_socket, buffer, BUFFER_SIZE, 0) > 0) {
            printf("Message from client: %s\n", buffer);
            memset(buffer, 0, BUFFER_SIZE);
        }

        close(client_socket);
    }

    close(server_socket);
    return 0;
}
