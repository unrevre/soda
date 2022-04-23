#ifndef UTIL_WORKER_H
#define UTIL_WORKER_H

#include <pthread.h>

#include <stdint.h>

struct worker_t {
    pthread_t thread;

    void* data;

    volatile int32_t flag;
};

int32_t send_worker(struct worker_t* w, void* (*f)(void*), void* data);
int32_t wait_worker(struct worker_t* w);
int32_t join_worker(struct worker_t* w);

#endif /* UTIL_WORKER_H */
