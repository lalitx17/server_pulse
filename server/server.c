#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int Server(server_t *serv, int port){
    if (!serv) return NULL;

    serv -> port = port;

    serv -> listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (serv -> listen_fd < 0){
        perror("socket");
        exit(EXIT_FAILURE);
    }
}
