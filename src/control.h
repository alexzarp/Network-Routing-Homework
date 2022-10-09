#ifndef CONTROL
#define CONTROL
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

typedef struct {
    int type;
    char *root;
    char *destiny;
    void *payload;
}Message;

typedef struct{
    Message **itens;
    int size;
    int head;
    sem_t *semaphore;
    pthread_mutex_t *mutex;
}Queue;

Queue *buildQueue(int buffer, pthread_mutex_t *mutex, sem_t *semaphore);
void enqueue(Queue *queue, Message *message);
Message *dequeue(Queue *queue);
void freeQueue(Queue *queue);

#endif /*CONTROL*/