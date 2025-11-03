#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>    // for usleep
#include <time.h>      // for srand/time

// Shared global variables
int A = 0;
int B = 0;

// Semaphores for protecting each variable
sem_t semA;
sem_t semB;

// Number of additions per thread
#define ITERATIONS 100

// Function declarations
void* producer(void* arg);
void* consumer(void* arg);

// Random sleep between 0 and 100 microseconds
void random_sleep() {
    int micro = rand() % 101;  // 0-100 us
    usleep(micro);
}

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

    // Initialize semaphores
    sem_init(&semA, 0, 1);
    sem_init(&semB, 0, 1);

    srand(time(NULL));  // Seed random number generator

    // Create threads (first half producers, second half consumers)
    for (int i = 0; i < num_threads; i++) {
        if (i < num_threads / 2)
            pthread_create(&threads[i], NULL, producer, NULL);
        else
            pthread_create(&threads[i], NULL, consumer, NULL);
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroy semaphores
    sem_destroy(&semA);
    sem_destroy(&semB);

    // Print final values of shared variables
    printf("Final value of A: %d\n", A);
    printf("Final value of B: %d\n", B);

    return 0;
}

// Producer thread: adds 1 to A, then 3 to B, 100 times
void* producer(void* arg) {
    pthread_t tid = pthread_self();
    printf("I am a producer, TID: %lu\n", (unsigned long)tid);

    for (int i = 0; i < ITERATIONS; i++) {
        sem_wait(&semA);
        A += 1;
        sem_post(&semA);
        random_sleep();

        sem_wait(&semB);
        B += 3;
        sem_post(&semB);
        random_sleep();
    }
    return NULL;
}

// Consumer thread: adds 3 to B, then 1 to A, 100 times
void* consumer(void* arg) {
    pthread_t tid = pthread_self();
    printf("I am a consumer, TID: %lu\n", (unsigned long)tid);

    for (int i = 0; i < ITERATIONS; i++) {
        sem_wait(&semB);
        B += 3;
        sem_post(&semB);
        random_sleep();

        sem_wait(&semA);
        A += 1;
        sem_post(&semA);
        random_sleep();
    }
    return NULL;
}
