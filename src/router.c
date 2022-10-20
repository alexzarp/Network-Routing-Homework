#include "control.h"

struct message{
    int type;
    char *root;
    char *destiny;
    int len;
    void *payload;
};

struct queue{
    Message **itens;
    int size;
    int head;
    int tail;
    sem_t *semaphore;
    pthread_mutex_t *mutex;
};

struct thread_arr{
    int socket;
    struct sockaddr_in *addrMe;
    Queue *controlQueue;
};

Message *buildMessage(int type, char *root, char *destiny, void *payload, int len){
    Message *msg = malloc(sizeof(*msg));
    msg->root = root;
    msg->destiny = destiny;
    msg->payload = payload;
    msg->type = type;
    msg->len = len;
    return msg;
}

void displayMessage(Message *msg){
    printf("\n------------------------------\n");
    printf("HEADER\n");
    printf("\tMessage Type: %d\n", msg->type);
    printf("\tRoot: %s\n", msg->root);
    printf("\tDestiny: %s\n", msg->destiny);
    printf("PAYLOAD\n\t%s\n", (char *)msg->payload);
    printf("------------------------------\n");
}

void freeMessage(Message *msg){
    free(msg->destiny);
    free(msg->root);
    free(msg->payload);
    free(msg);
}

ThreadArr *buildThreadConfig(int socket, struct sockaddr_in *addrMe, Queue *controlQueue){
    ThreadArr *new = malloc(sizeof(*new));
    new->socket = socket;
    new->addrMe = addrMe;
    new->controlQueue = controlQueue;
    return new;
}

Queue *buildQueue(int buffer){
    Queue *queue = malloc(sizeof(*queue));
    queue->itens = malloc(sizeof(Message *) * buffer);

    for(int i = 0; i < buffer; i++) queue->itens[i] = NULL;

    queue->size = buffer;
    queue->head = 0;
    queue->tail = 0;
    queue->mutex = malloc(sizeof(pthread_mutex_t));
    queue->semaphore = malloc(sizeof(sem_t));

    pthread_mutex_init(queue->mutex, NULL);
    sem_init(queue->semaphore, 0, 0);

    return queue;
}

static void compressQueue(Queue *queue){
    Message **new = malloc(sizeof(Message *) * queue->size);
    for(int i = 0; i < queue->size; i++) new[i] = NULL;
    
    int j = queue->head;
    for(int i = 0; i < queue->size && j < queue->size; i++){
        new[i] = queue->itens[j];
        j++;
    }
    queue->head = 0;
    free(queue->itens);
    queue->itens = new;
}

void enqueue(Queue *queue, Message *message){
    pthread_mutex_lock(queue->mutex);

    if(queue->tail == queue->size){
        if(queue->tail - queue->head != queue->size){
            queue->tail -= queue->head;
            compressQueue(queue);

            queue->itens[queue->tail] = message;
            ++queue->tail;

            sem_post(queue->semaphore);

            pthread_mutex_unlock(queue->mutex);

            return;
        }
        pthread_mutex_unlock(queue->mutex);    
        return;
    }

    queue->itens[queue->tail] = message;
    ++queue->tail;

    sem_post(queue->semaphore);

    pthread_mutex_unlock(queue->mutex);
}

Message *dequeue(Queue *queue){
    Message *buffer;

    sem_wait(queue->semaphore);

    pthread_mutex_lock(queue->mutex);

    buffer = queue->itens[queue->head];
    queue->itens[queue->head] = NULL;
    ++queue->head;

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

static char **stringSplit(char *payload, char *sep){
    char **result = NULL;
    int size = 0;

    for(int i = 0; payload[i] != '\0'; i++){
        size += (int)payload[i] == (int)sep[0] ? 1 : 0; 
    }

    result = calloc(size+1, sizeof(char *));

    char *token = strtok(payload, sep);
    result[0] = token;
    int i = 1;

    while(token != NULL){
        token = strtok(NULL, sep);
        result[i] = token;
        i++;
    }

    return result;
}

void *sender(void *config){
    struct sockaddr_in si_other;
    int slen = sizeof(si_other);
    ThreadArr *arr = (ThreadArr *)config;
    char buffer[BUFFER];

    while(1){
        memset(buffer,'\0', BUFFER);
        memset((char *) &si_other, 0, sizeof(si_other));
        Message *msg = dequeue(arr->controlQueue);

        char type[2];
        sprintf(type, "%d", msg->type);
        strcat(buffer, type);
        strcat(buffer, "|");
        strcat(buffer, msg->root);
        strcat(buffer, "|");
        strcat(buffer, msg->destiny);
        strcat(buffer, "|");
        strcat(buffer, (char *)msg->payload);

        char **adress = stringSplit((char *)msg->destiny, ":");
        si_other.sin_family = AF_INET;
        si_other.sin_port = htons(atoi(adress[2]));
        if (inet_aton(adress[0], &si_other.sin_addr) == 0) 
        {
            fprintf(stderr, "inet_aton() failed\n");
            exit(2);
        }

        if (sendto(arr->socket, buffer, strlen(buffer), 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            exit(2);
        }
    }
}

void *receiver(void *config){
    struct sockaddr_in sin_other;
    int slen = sizeof(sin_other);
    ThreadArr *arr = (ThreadArr *)config;
    char buffer[BUFFER];

    while(1){
        memset(buffer,'\0', BUFFER);

        if ((recvfrom(arr->socket, buffer, BUFFER, 0, (struct sockaddr *)&sin_other, &slen)) == -1)
        {
            exit(2);
        }

        char **result = stringSplit(buffer, "|");

        enqueue(arr->controlQueue, buildMessage((int)result[0][0] - 48, result[1], result[2], (void *)result[3], slen));
    }
    pthread_exit(NULL);
}