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

typedef struct message{
    int type;
    char *root;
    char *destiny;
    int len;
    void *payload;
}Message;

typedef struct {
    int socket;
    struct sockaddr_in *addrMe;
    Queue *controlQueue;
}ThreadArr;

typedef struct queue Queue;

// Messages Functions
Message *buildMessage(int type, char *root, char *destiny, void *payload, int len);
void displayMessage(Message *msg);
void freeMessage(Message *msg);

// Threads Config Functions
ThreadArr *buildThreadConfig(int socket, struct sockaddr_in *addrMe, Queue *controlQueue);

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