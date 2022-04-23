#include "worker.h"

#include <stddef.h>

int32_t send_worker(struct worker_t* w, void* (*f)(void*), void* data) {
    w->data = data;
    w->flag = 1;

    if (pthread_create(&w->thread, NULL, f, w))
        return 1;

    return 0;
}

int32_t wait_worker(struct worker_t* w) {
    if (pthread_join(w->thread, NULL))
        return 1;

    return 0;
}

int32_t join_worker(struct worker_t* w) {
    w->flag = 0;

    if (pthread_join(w->thread, NULL))
        return 1;

    return 0;
}
