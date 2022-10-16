#include "control.h"

Queue *buildQueue(int buffer, pthread_mutex_t *mutex, sem_t *semaphore){
    Queue *queue = malloc(sizeof(*queue));
    queue->itens = malloc(sizeof(Message *) * buffer);

    for(int i = 0; i < buffer; i++) queue->itens[i] = NULL;

    queue->size = buffer;
    queue->head = 0;
    queue->mutex = mutex;
    queue->semaphore = semaphore;

    return queue;
}

void enqueue(Queue *queue, Message *message){
    pthread_mutex_lock(queue->mutex);

    if(queue->head >= queue->size){
        pthread_mutex_unlock(queue->mutex);    
        return;
    }

    queue->itens[queue->head] = message;
    ++queue->head;

    pthread_mutex_unlock(queue->mutex);

    sem_post(queue->semaphore);
}

Message *dequeue(Queue *queue){
    Message *buffer;

    sem_wait(queue->semaphore);

    pthread_mutex_lock(queue->mutex);

    buffer = queue->itens[queue->head];
    --queue->head;

    pthread_mutex_unlock(queue->mutex);

    return buffer;
}

void freeQueue(Queue *queue){
    for(int i = 0; i < queue->head; i++)free(queue->itens[i]);
    free(queue->itens);
    pthread_mutex_destroy(queue->mutex);
    sem_destroy(queue->semaphore);
    free(queue);
}

void *sender(void *output);
void *messageHandle(void **queues);
void *receiver(void *input);