#include "../server/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080


int main(int argc, char *argv[]){
    int port = PORT;
    if (argc == 2){
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535){
            fprintf(stderr, "Invalid port number\n");
            return 1;
        }
    }

    server_t serv;
    if (Server(&serv, port) < 0){
        fprintf(stderr, "Failed to start server\n");
        return 1;
    }

    printf("Server started on port %d\n", port);

    while(1){
        int client_fd = client_accept(&serv);
        if (client_fd < 0){
            fprintf(stderr, "Failed to accept client\n");
            continue;
        }

        // Handle client request
        char buffer[1024];
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read < 0){
            fprintf(stderr, "Failed to read request\n");
            close(client_fd);
            continue;
        }
        buffer[bytes_read] = '\0';

       printf("%s", buffer);


        const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "Connection: close\r\n"
        "\r\n"
        "Hello World";

        write(client_fd, response, strlen(response));
        close(client_fd);
    }
}
