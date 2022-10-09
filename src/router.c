#include "control.h"

Queue *buildQueue(int buffer, pthread_mutex_t *mutex, sem_t *semaphore){
    Queue *queue = malloc(sizeof(*queue));
    queue->itens = malloc(sizeof(char *) * buffer);

    for(int i = 0; i < buffer; i++) queue->itens[i] = NULL;

    queue->size = buffer;
    queue->ocupation = 0;
    queue->mutex = mutex;
    queue->semaphore = semaphore;
    
    return queue;
}