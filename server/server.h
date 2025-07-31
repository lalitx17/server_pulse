#ifndef SERVER_H
#define SERVER_H

typedef struct server {
    int listen_fd;
    int port;
} server_t;

int Server(server_t *serv, int port);

int client_accept(server_t *serv);

int server_close(server_t *serv);

#endif // SERVER_H
