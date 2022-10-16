#ifndef CONTROL
#define CONTROL
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

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
    int tail;
    sem_t *semaphore;
    pthread_mutex_t *mutex;
}Queue;

// Messages Functions
Message *buildMessage(int type, char *root, char *destiny, void *payload);
void displayMessage(Message *msg);


// Queues Functions
Queue *buildQueue(int buffer);
void enqueue(Queue *queue, Message *message);
Message *dequeue(Queue *queue);
void freeQueue(Queue *queue);

//Threads
void *sender(void *output);
void *messageHandle(void *queues);
void *receiver(void *input);

#endif /*CONTROL*/