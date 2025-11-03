#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define ITERATIONS 100       // Part B iterations
#define BUFFER_SIZE 3        // Part C buffer size
#define PRODUCE_CONSUME 20   // Part C per-thread operations

// Part B shared variables
int A = 0;
int B = 0;

// Semaphores for A and B
sem_t semA;
sem_t semB;

// Part C: bounded buffer
int buffer[BUFFER_SIZE];
int in = 0;   // index for next produce
int out = 0;  // index for next consume
sem_t empty;  // empty slots
sem_t full;   // full slots
pthread_mutex_t mutex; // protects buffer access

// Function declarations
void* producer_partB(void* arg);
void* consumer_partB(void* arg);
void* producer_partC(void* arg);
void* consumer_partC(void* arg);
void random_sleep() {
    int micro = rand() % 101; // 0-100 us
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

    // --- Part B Initialization ---
    sem_init(&semA, 0, 1);
    sem_init(&semB, 0, 1);
    srand(time(NULL));

    // Create Part B threads
    for (int i = 0; i < num_threads; i++) {
        if (i < num_threads / 2)
            pthread_create(&threads[i], NULL, producer_partB, NULL);
        else
            pthread_create(&threads[i], NULL, consumer_partB, NULL);
    }

    // Wait for Part B threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&semA);
    sem_destroy(&semB);

    printf("\nPart B:\nA = %d\nB = %d\n\n", A, B);

    // --- Part C Initialization ---
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    // Reset threads for Part C
    for (int i = 0; i < num_threads; i++) {
        if (i < num_threads / 2)
            pthread_create(&threads[i], NULL, producer_partC, NULL);
        else
            pthread_create(&threads[i], NULL, consumer_partC, NULL);
    }

    // Wait for Part C threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup Part C semaphores and mutex
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return 0;
}

// ------------------- Part B Threads -------------------
void* producer_partB(void* arg) {
    pthread_t tid = pthread_self();
    for (int i = 0; i < ITERATIONS; i++) {
        sem_wait(&semA); A += 1; sem_post(&semA); random_sleep();
        sem_wait(&semB); B += 3; sem_post(&semB); random_sleep();
    }
    return NULL;
}

void* consumer_partB(void* arg) {
    pthread_t tid = pthread_self();
    for (int i = 0; i < ITERATIONS; i++) {
        sem_wait(&semB); B += 3; sem_post(&semB); random_sleep();
        sem_wait(&semA); A += 1; sem_post(&semA); random_sleep();
    }
    return NULL;
}

// ------------------- Part C Threads -------------------
void* producer_partC(void* arg) {
    pthread_t tid = pthread_self();
    for (int i = 0; i < PRODUCE_CONSUME; i++) {
        int item = rand() % 100; // random number to produce

        sem_wait(&empty);       // wait for empty slot
        pthread_mutex_lock(&mutex);
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        printf("%lu: Produces %d\n", (unsigned long)tid, item);
        pthread_mutex_unlock(&mutex);
        sem_post(&full);        // signal full slot
        usleep(rand() % 101);   // random sleep
    }
    return NULL;
}

void* consumer_partC(void* arg) {
    pthread_t tid = pthread_self();
    for (int i = 0; i < PRODUCE_CONSUME; i++) {
        sem_wait(&full);        // wait for item
        pthread_mutex_lock(&mutex);
        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        printf("%lu: Consumes %d\n", (unsigned long)tid, item);
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);       // signal empty slot
        usleep(rand() % 101);   // random sleep
    }
    return NULL;
}
