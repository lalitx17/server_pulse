#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int Server(server_t *serv, int port){
    if (!serv) return -1;

    serv -> port = port;

    serv -> listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (serv -> listen_fd < 0){
        perror("socket");
        close(serv -> listen_fd);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serv -> listen_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0){
        perror("bind");
        close(serv -> listen_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(serv -> listen_fd, 1) < 0){
        perror("listen");
        close(serv -> listen_fd);
        exit(EXIT_FAILURE);
    }

    return 0;
}

int client_accept(server_t *serv){
    if (!serv || serv -> listen_fd < 0) return -1;

    int client_fd = accept(serv -> listen_fd, NULL, NULL);

    if (client_fd < 0){
        perror("accept");
        close(serv -> listen_fd);
        exit(EXIT_FAILURE);
    }

    return client_fd;
}
