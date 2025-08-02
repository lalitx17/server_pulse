#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include "../request/request.h"
#include <pthread.h>
#include <stdbool.h>

typedef struct task {
    int client_fd;
    request_t *request;
    struct task *next;
} task_t;

typedef struct task_queue {
    task_t *head;
    task_t *tail;
    int size;
    int capacity;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    bool shutdown;
} task_queue_t;

int task_queue_init(task_queue_t *queue, int capacity);
int task_queue_enqueue(task_queue_t *queue, int client_fd, request_t *request);
int task_queue_dequeue(task_queue_t *queue, int *client_fd,
                       request_t **request);
int task_queue_size(task_queue_t *queue);
int task_queue_is_empty(task_queue_t *queue);
int task_queue_is_full(task_queue_t *queue);
int task_queue_shutdown(task_queue_t *queue);
int task_queue_destroy(task_queue_t *queue);

#endif