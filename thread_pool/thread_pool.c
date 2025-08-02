#include "thread_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *worker_thread(void *arg) {
    thread_pool_t *pool = (thread_pool_t *)arg;
    while (!pool->shutdown) {
        pool->worker_function(pool->worker_arg);
    }
    return NULL;
}

int thread_pool_init(thread_pool_t *pool, int thread_count,
                     worker_function_t worker_function, void *worker_arg) {

    if (!pool || thread_count <= 0 || !worker_function) {
        return -1;
    }

    pool->threads = (pthread_t *)malloc(thread_count * sizeof(pthread_t));
    if (!pool->threads) {
        perror("Failed to allocate memory for threads");
        return -1;
    }

    pool->thread_count = thread_count;
    pool->worker_function = worker_function;
    pool->worker_arg = worker_arg;
    pool->shutdown = false;

    for (int i = 0; i < thread_count; i++) {
        if (pthread_create(&pool->threads[i], NULL, worker_thread, pool) != 0) {
            perror("Failed to create worker thread");
            for (int j = 0; j < i; j++) {
                pthread_join(pool->threads[j], NULL);
            }
            free(pool->threads);
            return -1;
        }
    }

    printf("Thread pool initialized with %d threads\n", thread_count);
    return 0;
}

void thread_pool_shutdown(thread_pool_t *pool) {
    if (!pool) {
        return;
    }

    pool->shutdown = true;
    for (int i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    printf("Thread pool shutdown\n");
}

void thread_pool_destroy(thread_pool_t *pool) {
    if (!pool) {
        return;
    }

    thread_pool_shutdown(pool);
    free(pool->threads);
    printf("Thread pool destroyed\n");
}
