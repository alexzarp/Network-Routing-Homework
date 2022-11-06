#ifndef CONTROL
#define CONTROL

#include <pthread.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rof.h"
#include "message.h"
#include "thread.h"

#define BUFFER 100
#define QUEUESIZE 10

typedef struct queue Queue;

// Queues Functions
Queue *buildQueue(int buffer);
void enqueue(Queue *queue, Message *message);
Message *dequeue(Queue *queue);
void freeQueue(Queue *queue);

//Router Functions
int buildRouter (Router *router);
void die(char *s);

#endif /*CONTROL*/