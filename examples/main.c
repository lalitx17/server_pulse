#include "../request/request.h"
#include "../server/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

void check_error(int ret, const char *msg) {
    if (ret < 0) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    int port = PORT;
    if (argc == 2) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Invalid port number\n");
            return -1;
        }
    }

    server_t serv;
    if (Server(&serv, port) < 0) {
        fprintf(stderr, "Failed to start server\n");
        return -1;
    }

    printf("Server started on port %d\n", port);

    while (1) {
        int client_fd = client_accept(&serv);
        check_error(client_fd, "client fd");

        char buffer[1024];
        request_t *request = malloc(sizeof(request_t));
        if (!request) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        memset(request, 0, sizeof(request_t));
        check_error(request_init(client_fd, buffer, sizeof(buffer)),
                    "request init");
        int parse_result = request_parse(buffer, request);
        if (parse_result == 0) {
            check_error(print_request(request), "print request");
        } else {
            fprintf(stderr, "Failed to parse request\n");
        }
        request_destroy(request);
        free(request);

        const char *response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/html\r\n"
                               "Content-Length: 45\r\n"
                               "Connection: close\r\n"
                               "\r\n"
                               "<h1>Hello World</h1><p>This is HTML!</p>\r\n";

        check_error(write(client_fd, response, strlen(response)), "write");
        shutdown(client_fd, SHUT_WR);
        close(client_fd);
    }
}
