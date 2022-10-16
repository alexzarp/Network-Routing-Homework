#include "control.h"

void displayMessage(Message *msg){
    printf("\n------------------------------\n");
    printf("HEADER\n");
    printf("\tMessage Type: %d\n", msg->type);
    printf("\tRoot: %s\n", msg->root);
    printf("\tDestiny: %s\n", msg->destiny);
    printf("PAYLOAD\n\t%s\n", (char *)msg->payload);
    printf("------------------------------\n");
}

Message *buildMessage(int type, char *root, char *destiny, void *payload){
    Message *msg = malloc(sizeof(*msg));
    msg->root = root;
    msg->destiny = destiny;
    msg->payload = payload;
    msg->type = type;
    return msg;
}

Queue *buildQueue(int buffer){
    Queue *queue = malloc(sizeof(*queue));
    queue->itens = malloc(sizeof(Message *) * buffer);

    for(int i = 0; i < buffer; i++) queue->itens[i] = NULL;

    queue->size = buffer;
    queue->head = 0;
    queue->mutex = malloc(sizeof(pthread_mutex_t));
    queue->semaphore = malloc(sizeof(sem_t));

    pthread_mutex_init(queue->mutex, NULL);
    sem_init(queue->semaphore, 0, 0);

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

    sem_post(queue->semaphore);

    pthread_mutex_unlock(queue->mutex);
}

Message *dequeue(Queue *queue){
    Message *buffer;

    sem_wait(queue->semaphore);

    pthread_mutex_lock(queue->mutex);

    buffer = queue->itens[queue->head - 1];
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