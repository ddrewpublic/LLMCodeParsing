// File: tiny_rest_api.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to handle HTTP requests
void handle_request(int new_socket) {
    char buffer[BUFFER_SIZE];
    read(new_socket, buffer, sizeof(buffer) - 1);
    
    // Simple parsing of the HTTP request
    if (strncmp(buffer, "GET /hello", 10) == 0) {
        const char *response = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nHello, World!";
        send(new_socket, response, strlen(response), 0);
    } else if (strncmp(buffer, "GET /goodbye", 12) == 0) {
        const char *response = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nGoodbye, World!";
        send(new_socket, response, strlen(response), 0);
    } else {
        const char *response = "HTTP/1.1 404 Not Found\nContent-Type: text/plain\n\n404 Not Found";
        send(new_socket, response, strlen(response), 0);
    }

    close(new_socket);
}

// Main function to set up the server
int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Main loop to accept and handle requests
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        handle_request(new_socket);
    }

    return 0;
}
