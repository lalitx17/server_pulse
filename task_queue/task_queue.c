#include "task_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int task_queue_init(task_queue_t *queue, int capacity) {
    if (queue == NULL || capacity <= 0) {
        return -1;
    }

    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    queue->capacity = capacity;
    queue->shutdown = false;

    if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
        return -1;
    }

    if (pthread_cond_init(&queue->not_empty, NULL) != 0) {
        pthread_mutex_destroy(&queue->mutex);
        return -1;
    }

    if (pthread_cond_init(&queue->not_full, NULL) != 0) {
        pthread_cond_destroy(&queue->not_empty);
        pthread_mutex_destroy(&queue->mutex);
    }

    return 0;
}

int task_queue_enqueue(task_queue_t *queue, int client_fd, request_t *request) {
    if (!queue || !request) {
        return -1;
    }

    pthread_mutex_lock(&queue->mutex);

    while (queue->size >= queue->capacity && !queue->shutdown) {
        pthread_cond_wait(&queue->not_full, &queue->mutex);
    }

    if (queue->shutdown) {
        pthread_mutex_unlock(&queue->mutex);
        return -1;
    }

    task_t *new_task = (task_t *)malloc(sizeof(task_t));
    if (!new_task) {
        pthread_mutex_unlock(&queue->mutex);
        return -1;
    }

    new_task->client_fd = client_fd;

    new_task->request->method =
        request->method ? strdup(request->method) : NULL;
    new_task->request->url = request->url ? strdup(request->url) : NULL;
    new_task->request->version =
        request->version ? strdup(request->version) : NULL;
    new_task->request->body = request->body ? strdup(request->body) : NULL;
    new_task->request->n_headers = request->n_headers;

    if (request->n_headers > 0 && request->headers) {
        new_task->request->headers =
            (header_t *)malloc(request->n_headers * sizeof(header_t));
        if (!new_task->request->headers) {
            free(new_task->request->method);
            free(new_task->request->url);
            free(new_task->request->version);
            free(new_task->request->body);
            free(new_task);
            pthread_mutex_unlock(&queue->mutex);
            return -1;
        }

        for (int i = 0; i < request->n_headers; i++) {
            new_task->request->headers[i].name =
                request->headers[i].name ? strdup(request->headers[i].name)
                                         : NULL;
            new_task->request->headers[i].value =
                request->headers[i].value ? strdup(request->headers[i].value)
                                          : NULL;
        }
    } else {
        new_task->request->headers = NULL;
    }
    new_task->next = NULL;

    if (queue->tail) {
        queue->tail->next = new_task;
    } else {
        queue->head = new_task;
    }
    queue->tail = new_task;
    queue->size++;

    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);

    printf("Task enqueued: %s %s %s\n", request->method, request->url,
           request->version);

    return 0;
}

int task_queue_dequeue(task_queue_t *queue, int *client_fd,
                       request_t *request) {
    if (!queue || !client_fd || !request) {
        return -1;
    }

    pthread_mutex_lock(&queue->mutex);

    while (queue->size == 0 && !queue->shutdown) {
        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }

    if (queue->size == 0 && queue->shutdown) {
        pthread_mutex_unlock(&queue->mutex);
        return -1;
    }

    task_t *task = queue->head;
    queue->head = task->next;

    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    queue->size--;

    *client_fd = task->client_fd;

    request->method =
        task->request->method ? strdup(task->request->method) : NULL;
    request->url = task->request->url ? strdup(task->request->url) : NULL;
    request->version =
        task->request->version ? strdup(task->request->version) : NULL;
    request->body = task->request->body ? strdup(task->request->body) : NULL;
    request->n_headers = task->request->n_headers;

    if (task->request->n_headers > 0 && task->request->headers) {
        request->headers =
            (header_t *)malloc(task->request->n_headers * sizeof(header_t));
        if (request->headers) {
            for (int i = 0; i < task->request->n_headers; i++) {
                request->headers[i].name =
                    task->request->headers[i].name
                        ? strdup(task->request->headers[i].name)
                        : NULL;
                request->headers[i].value =
                    task->request->headers[i].value
                        ? strdup(task->request->headers[i].value)
                        : NULL;
            }
        }
    } else {
        request->headers = NULL;
    }

    request_destroy(task->request);
    free(task);

    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);

    printf("Task dequeued: %s %s %s\n", request->method, request->url,
           request->version);

    return 0;
}

int task_queue_size(task_queue_t *queue) {
    if (!queue) {
        return -1;
    }

    pthread_mutex_lock(&queue->mutex);
    int size = queue->size;
    pthread_mutex_unlock(&queue->mutex);
    return size;
}

bool task_queue_is_empty(task_queue_t *queue) {
    if (!queue) {
        return true;
    }

    pthread_mutex_lock(&queue->mutex);
    bool empty = queue->size == 0;
    pthread_mutex_unlock(&queue->mutex);
    return empty;
}

bool task_queue_is_full(task_queue_t *queue) {
    if (!queue) {
        return true;
    }

    pthread_mutex_lock(&queue->mutex);
    bool full = queue->size >= queue->capacity;
    pthread_mutex_unlock(&queue->mutex);
    return full;
}

void task_queue_shutdown(task_queue_t *queue) {
    if (!queue) {
        return;
    }

    pthread_mutex_lock(&queue->mutex);
    queue->shutdown = true;
    pthread_cond_broadcast(&queue->not_empty);
    pthread_cond_broadcast(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);
}

void task_queue_destroy(task_queue_t *queue) {
    if (!queue) {
        return;
    }
    task_queue_shutdown(queue);

    pthread_mutex_lock(&queue->mutex);
    task_t *current = queue->head;
    while (current) {
        task_t *next = current->next;
        request_destroy(current->request);
        free(current);
        current = next;
    }
    pthread_mutex_unlock(&queue->mutex);

    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
    free(queue);
}
