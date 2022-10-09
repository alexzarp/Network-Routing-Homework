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
    char **itens;
    int size;
    int ocupation;
    sem_t *semaphore;
    pthread_mutex_t *mutex;
}Queue;

Queue *buildQueue(int buffer, pthread_mutex_t *mutex, sem_t *semaphore);

#endif /*CONTROL*/