#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Function declarations
void* producer(void* arg);
void* consumer(void* arg);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <even number of threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);

    if (num_threads <= 0 || num_threads % 2 != 0) {
        printf("Error: Please enter a positive even number of threads.\n");
        return 1;
    }

    pthread_t threads[num_threads];

    // Create threads
    for (int i = 0; i < num_threads; i++) {
        if (i < num_threads / 2) {
            // First half are producers
            pthread_create(&threads[i], NULL, producer, NULL);
        } else {
            // Second half are consumers
            pthread_create(&threads[i], NULL, consumer, NULL);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

// Producer thread function
void* producer(void* arg) {
    pthread_t tid = pthread_self();
    printf("I am a producer, TID: %lu\n", (unsigned long)tid);
    return NULL;
}

// Consumer thread function
void* consumer(void* arg) {
    pthread_t tid = pthread_self();
    printf("I am a consumer, TID: %lu\n", (unsigned long)tid);
    return NULL;
}
