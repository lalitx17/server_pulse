#include "server.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int Server(server_t *serv, int port) {
    printf("Server() called with port %d\n", port);

    if (!serv) {
        printf("ERROR: serv is NULL\n");
        return -1;
    }

    serv->port = port;
    serv->routes_count = 0;
    serv->listen_fd = -1;
    serv->routes = NULL;
    serv->routes_capacity = 0;

    printf("Initializing routes...\n");
    if (routes_init(&serv->routes, &serv->routes_capacity) != 0) {
        printf("ERROR: routes_init failed\n");
        return -1;
    }
    printf("Routes initialized successfully. Capacity: %d\n",
           serv->routes_capacity);

    printf("Creating socket...\n");
    serv->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("socket() returned fd: %d\n", serv->listen_fd);

    if (serv->listen_fd < 0) {
        printf("ERROR: socket() failed: %s\n", strerror(errno));
        perror("socket");
        routes_free(serv->routes);
        return -1;
    }

    int opt = 1;
    printf("Setting socket options...\n");
    if (setsockopt(serv->listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt,
                   sizeof(opt)) < 0) {
        printf("ERROR: setsockopt failed: %s\n", strerror(errno));
        perror("setsockopt");
        close(serv->listen_fd);
        routes_free(serv->routes);
        return -1;
    }
    printf("Socket options set successfully\n");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    printf("Binding to port %d...\n", port);
    if (bind(serv->listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("ERROR: bind() failed: %s\n", strerror(errno));
        perror("bind");
        close(serv->listen_fd);
        routes_free(serv->routes);
        return -1;
    }
    printf("Bind successful\n");

    printf("Starting to listen (backlog=10)...\n");
    if (listen(serv->listen_fd, 10) < 0) {
        printf("ERROR: listen() failed: %s\n", strerror(errno));
        perror("listen");
        close(serv->listen_fd);
        routes_free(serv->routes);
        return -1;
    }
    printf("Listen successful\n");

    printf("Server initialization complete. listen_fd = %d\n", serv->listen_fd);
    return 0;
}

int client_accept(server_t *serv) {
    printf("client_accept() called\n");

    if (!serv) {
        printf("ERROR: serv is NULL\n");
        return -1;
    }

    printf("Checking listen_fd: %d\n", serv->listen_fd);
    if (serv->listen_fd < 0) {
        printf("ERROR: listen_fd is invalid (%d)\n", serv->listen_fd);
        return -1;
    }

    printf("Calling accept() on fd %d...\n", serv->listen_fd);
    int client_fd = accept(serv->listen_fd, NULL, NULL);
    printf("accept() returned: %d\n", client_fd);

    if (client_fd < 0) {
        printf("ERROR: accept() failed: %s\n", strerror(errno));
        perror("accept");
        return -1;
    }

    printf("Client accepted successfully with fd: %d\n", client_fd);
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