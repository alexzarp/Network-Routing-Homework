#ifndef CONTROL
#define CONTROL

#include <pthread.h>
#include <semaphore.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER 300

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

typedef struct{
    int socket;
    int *socketLen;
    struct sockaddr_in *addrMe;
    struct sockaddr_in *addrOther;
    Queue *controlQueue;
}ThreadArr;

// Messages Functions
Message *buildMessage(int type, char *root, char *destiny, void *payload);
void displayMessage(Message *msg);
void freeMessage(Message *msg);

// Threads Config Functions
ThreadArr *buildThreadConfig(int socket, int *socketLen, struct sockaddr_in *addrMe, struct sockaddr_in *addrOther, Queue *controlQueue);

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