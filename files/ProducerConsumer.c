#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <assert.h>
#include "../headers/LinkedList.h"


// 2 producers, 2 consumers | 2 threads each
#define THREAD_NUM 4
#define MAX_BUFFER_SIZE 50

sem_t semEmpty;
sem_t semFull;

pthread_mutex_t mutexBuffer;

Node* head = NULL;

int s = 42;

// Tracking Variables
int seq = 0;
int totalProduced = 0;
int totalConsumed = 0;
int producersDone = 0;


void* producerOdd(void* args) {
    FILE *logProducerOdd = fopen("./logs/producer_odd_output.txt", "w");

    int oddProduced = 0;
    while (1) {
        sleep(.5);

        // Check to see if done
        pthread_mutex_lock(&mutexBuffer);
        if (totalProduced >= 200 || oddProduced >= 100) {
            pthread_mutex_unlock(&mutexBuffer);
            break;
        }
        pthread_mutex_unlock(&mutexBuffer);

        int x = (rand() % 50) * 2 + 1; // odd

        // decrement the semEmpty semaphore
        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        
        // Log before
        char* listToStringBefore = listToString(head);
        fprintf(logProducerOdd, "[seq = %d] [thread = producer_odd] [action = before_insert] [value = %d] [list = %s]\n", seq, x, listToStringBefore);
        free(listToStringBefore);

        insertSortedNode(&head, x);
        totalProduced++;
        oddProduced++;
        seq++;
        
        // Log after
        char* listToStringAfter = listToString(head);
        fprintf(logProducerOdd, "[seq = %d] [thread = producer_odd] [action = after_insert]  [value = %d] [list = %s]\n\n", seq, x, listToStringAfter);
        free(listToStringAfter);
        
        // Increment the semFull semaphore
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);
    }

    fclose(logProducerOdd);

    // Checking to see if this was the last producer to finish
    if (producersDone++ == 2) {
        sem_post(&semFull);
        sem_post(&semFull);
    }
    return NULL;
}

void* producerEven(void* args) {
    FILE *logProducerEven = fopen("./logs/producer_even_output.txt", "w");
    int evenProduced = 0;
    while (1) {
        sleep(.5);
        pthread_mutex_lock(&mutexBuffer);
        if (totalProduced >= 200 || evenProduced >= 100) {
            pthread_mutex_unlock(&mutexBuffer);
            break;
        }
        pthread_mutex_unlock(&mutexBuffer);

        int x = (rand() % 50) * 2 + 2; // even


        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        
        // Log before
        char* listToStringBefore = listToString(head);
        fprintf(logProducerEven, "[seq = %d] [thread = producer_even] [action = before_insert] [value = %d] [list = %s]\n", seq, x, listToStringBefore);
        free(listToStringBefore);
            
        insertSortedNode(&head, x);
        totalProduced++;
        evenProduced++;
        seq++;
            
        // Log after
        char* listToStringAfter = listToString(head);
        fprintf(logProducerEven, "[seq = %d] [thread = producer_even] [action = after_insert]  [value = %d] [list = %s]\n\n", seq, x, listToStringAfter);
        free(listToStringAfter);
        
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);
    }

    fclose(logProducerEven);

    if (producersDone++ == 2) {
        sem_post(&semFull);
        sem_post(&semFull);
    }
    return NULL;
}

void* consumerOdd(void* args) {
    FILE *logConsumerOdd = fopen("./logs/consumer_odd_output.txt", "w");

    int oddConsumed = 0;
    while (1) {
        sleep(.5);

        // Check for completion before waiting
        pthread_mutex_lock(&mutexBuffer);
        int done = (producersDone >= 2 && head == NULL); // favorite syntax
        pthread_mutex_unlock(&mutexBuffer);
        if (done) {
            break;
        }

        sem_wait(&semFull);

        pthread_mutex_lock(&mutexBuffer);
        if ((producersDone >= 2 && head == NULL)) {
            // All producers finished and buffer empty: exit without touching semaphores
            pthread_mutex_unlock(&mutexBuffer);
            break;
        }

        if (head && head->data % 2 != 0) {
            // Log before
            char* listToStringBefore = listToString(head);
            fprintf(logConsumerOdd, "[seq = %d] [thread = consumer_odd] [action = before_delete] [value = %d] [list = %s]\n", seq, head->data, listToStringBefore);
            free(listToStringBefore);

            int value = head->data;
            deleteNode(&head, value);
            oddConsumed++;
            totalConsumed++;
            seq++;
            
            // Log after
            char* listToStringAfter = listToString(head);
            fprintf(logConsumerOdd, "[seq = %d] [thread = consumer_odd] [action = after_delete]  [value = %d] [list = %s]\n\n", seq, value, listToStringAfter);
            free(listToStringAfter);
            
            pthread_mutex_unlock(&mutexBuffer);
            sem_post(&semEmpty);
        } else {
            // Not our parity (or list changed). We didn't consume, so return the semFull token.
            sem_post(&semFull);
            pthread_mutex_unlock(&mutexBuffer);
        }
    }

    fclose(logConsumerOdd);
    return NULL;
}

void* consumerEven(void* args) {
    FILE *logConsumerEven = fopen("./logs/consumer_even_output.txt", "w");

    int evenConsumed = 0;
    while (1) {
        sleep(.5);
        pthread_mutex_lock(&mutexBuffer);
        int done = (producersDone >= 2 && head == NULL);
        pthread_mutex_unlock(&mutexBuffer);
        if (done) {
            break;
        }

        sem_wait(&semFull);

        pthread_mutex_lock(&mutexBuffer);
        if ((producersDone >= 2 && head == NULL)) {
            pthread_mutex_unlock(&mutexBuffer);
            break;
        }

        if (head && head->data % 2 == 0) {
            // Log before
            char* listToStringBefore = listToString(head);
            fprintf(logConsumerEven, "[seq = %d] [thread = consumer_even] [action = before_delete] [value = %d] [list = %s]\n", seq, head->data, listToStringBefore);
            free(listToStringBefore);
            
            int value = head->data;
            deleteNode(&head, value);
            evenConsumed++;
            totalConsumed++;
            seq++;
            
            // Log after
            char* listToStringAfter = listToString(head);
            fprintf(logConsumerEven, "[seq = %d] [thread = consumer_even] [action = after_delete]  [value = %d] [list = %s]\n\n", seq, value, listToStringAfter);
            free(listToStringAfter);
            
            pthread_mutex_unlock(&mutexBuffer);
            sem_post(&semEmpty);
        } else {
            sem_post(&semFull);
            pthread_mutex_unlock(&mutexBuffer);
        }
    }

    fclose(logConsumerEven);
    return NULL;
}



int main(int argc, char* argv[]) {
    if (argc > 2) {
        sscanf(argv[1], "%d", &s);
    }
    srand(s); //default seed=42
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&semEmpty, 0, MAX_BUFFER_SIZE);
    sem_init(&semFull, 0, 0);
    int i = 0;
    //hardcoded thread creation, might go back and make it dynanmic.
    pthread_create(&th[i], NULL, &producerOdd, NULL);
    pthread_create(&th[i+1], NULL, &producerEven, NULL);
    pthread_create(&th[i+2], NULL, &consumerOdd, NULL);
    pthread_create(&th[i+3], NULL, &consumerEven, NULL);
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    sem_destroy(&semEmpty);
    sem_destroy(&semFull);
    pthread_mutex_destroy(&mutexBuffer);
    return 0;
}