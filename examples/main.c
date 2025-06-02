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
