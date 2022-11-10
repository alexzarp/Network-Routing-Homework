#ifndef THRD
#define THRD

#include "control.h"

typedef struct queue Queue;

typedef struct {
    int rid;
    int socket;
    Queue *outputQueue;
    Queue *inputQueue;
}ThreadConfig;

ThreadConfig *buildThreadConfig(int rid, int socket, Queue *outputQueue, Queue *inputQueue);

void *sender(void *output);
void *packetHandler (void *config);
void *receiver(void *input);
void *terminal (void *config);

#endif /*THRD*/