#pragma once

/*

Apple compiler does not have barrier then here is the implementation using mutex and condition variable
THANX APPLE !

*/



#ifdef __APPLE__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    pthread_mutex_t mutex; // Protects access to the value and the condition variable
    pthread_cond_t cond;   // Signals a change of the value
    int count;             // The count of threads to wait for
    int current;           // The current count of waiting threads
} pthread_barrier_t;

int pthread_barrier_init(pthread_barrier_t *barrier, int count);

int pthread_barrier_wait(pthread_barrier_t *barrier);

void pthread_barrier_destroy(pthread_barrier_t *barrier);

#endif
