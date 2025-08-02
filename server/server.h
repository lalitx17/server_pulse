#ifndef SERVER_H
#define SERVER_H

#include "../route/route.h"
#include "../task_queue/task_queue.h"
#include "../thread_pool/thread_pool.h"

typedef struct server {
    int listen_fd;
    int port;
    route_t *routes;
    int routes_count;
    int routes_capacity;
    task_queue_t task_queue;
    thread_pool_t thread_pool;
} server_t;

int Server(server_t *serv, int port);
int client_accept(server_t *serv);
int server_close(server_t *serv);
int server_enqueue_task(server_t *serv, int client_fd, request_t *request);

#endif // SERVER_H
