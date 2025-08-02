#ifndef THREAD_POOL_C
#define THREAD_POOL_C

#include "thread_pool.h"
#include <pthread.h>
#include <stdbool.h>

typedef void (*worker_function_t)(void *arg);

typedef struct thread_pool {
    pthread_t *threads;
    int thread_count;
    worker_function_t worker_function;
    void *worker_arg;
    bool shutdown;
} thread_pool_t;

int thread_pool_init(thread_pool_t *pool, int thread_count,
                     worker_function_t worker_function, void *worker_arg);
void thread_pool_shutdown(thread_pool_t *pool);
void thread_pool_destroy(thread_pool_t *pool);

#endif // THREAD_POOL_C
