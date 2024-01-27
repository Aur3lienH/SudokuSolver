#include "deepLearning/Barrier.h"
#ifdef __clang__

    int pthread_barrier_init(pthread_barrier_t *barrier, int count) {
        int err = 0;
        if (count == 0) {
            return -1;
        }

        barrier->count = count;
        barrier->current = 0;

        err = pthread_mutex_init(&barrier->mutex, NULL);
        if (err != 0) return err;

        err = pthread_cond_init(&barrier->cond, NULL);
        if (err != 0) {
            pthread_mutex_destroy(&barrier->mutex); // Cleanup on error
            return err;
        }

        return 0;
    }

    int pthread_barrier_wait(pthread_barrier_t *barrier) {
        pthread_mutex_lock(&barrier->mutex);

        barrier->current++;
        if (barrier->current >= barrier->count) {
            barrier->current = 0; // Reset for the next use
            pthread_cond_broadcast(&barrier->cond); // Wake up all waiting threads
        } else {
            // If not all threads have reached, wait for a signal
            pthread_cond_wait(&barrier->cond, &barrier->mutex);
        }

        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    }

    void pthread_barrier_destroy(pthread_barrier_t *barrier) {
        // Destroy resources after use
        pthread_mutex_destroy(&barrier->mutex);
        pthread_cond_destroy(&barrier->cond);
    }

#endif