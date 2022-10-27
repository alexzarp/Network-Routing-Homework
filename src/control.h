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
#define QUEUESIZE 10

typedef struct message{
    int type;
    char *root;
    char *destiny;
    int len;
    void *payload;
}Message;

typedef struct queue Queue;

typedef struct {
    int socket;
    struct sockaddr_in *addrMe;
    Queue *outputQueue;
    Queue *inputQueue;
}ThreadConfig;

// Messages Functions
Message *buildMessage(int type, char *root, char *destiny, void *payload, int len);
void displayMessage(Message *msg);
void freeMessage(Message *msg);

// Threads Config Functions
ThreadConfig *buildThreadConfig(int socket, struct sockaddr_in *addrMe, Queue *outputQueue, Queue *inputQueue);

// Queues Functions
Queue *buildQueue(int buffer);
void enqueue(Queue *queue, Message *message);
Message *dequeue(Queue *queue);
void freeQueue(Queue *queue);

//Threads
void *sender(void *output);
void *packet_handler (void *config);
void *receiver(void *input);
void *terminal (void *config);

#endif /*CONTROL*/