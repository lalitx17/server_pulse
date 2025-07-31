#include "server.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int Server(server_t *serv, int port) {
    if (!serv)
        return -1;

    serv->port = port;
    serv->routes_count = 0;

    if (routes_init(&serv->routes, &serv->routes_capacity) != 0) {
        return -1;
    }

    serv->listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (serv->listen_fd < 0) {
        perror("socket");
        routes_free(serv->routes);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serv->listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(serv->listen_fd);
        routes_free(serv->routes);
        exit(EXIT_FAILURE);
    }

    if (listen(serv->listen_fd, 1) < 0) {
        perror("listen");
        close(serv->listen_fd);
        routes_free(serv->routes);
        exit(EXIT_FAILURE);
    }

    return 0;
}

int client_accept(server_t *serv) {
    if (!serv || serv->listen_fd < 0)
        return -1;

    int client_fd = accept(serv->listen_fd, NULL, NULL);

    if (client_fd < 0) {
        perror("accept");
        close(serv->listen_fd);
        exit(EXIT_FAILURE);
    }

    return client_fd;
}

int server_close(server_t *serv) {
    if (!serv)
        return -1;

    if (serv->listen_fd >= 0) {
        close(serv->listen_fd);
        serv->listen_fd = -1;
    }

    if (serv->routes) {
        routes_free(serv->routes);
        serv->routes = NULL;
        serv->routes_count = 0;
        serv->routes_capacity = 0;
    }

    return 0;
}
