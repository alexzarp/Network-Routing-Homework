#include "control.h"
#include "rof.h"

struct queue{
    Message **itens;
    int size;
    int head;
    int tail;
    sem_t *semaphore;
    pthread_mutex_t *mutex;
};

struct router{
    char *id;
    char *port;
    char *ip;
};

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

void die(char *s){
    perror(s);
    exit(1);
}
 

// buiuldRouter(rrouter(int num));
int buildRouter (Router *router) {
    struct sockaddr_in si_me, si_other;

    int s, i, slen = sizeof(si_other) , recv_len;
    char buf[BUFFER];

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        die("socket");
    }

    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(atoi(router->port));

    if (inet_aton(router->ip , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    // por enquanto, o id serve só pra leitura?

    if (bind(s, (struct sockaddr *)&si_me, sizeof(si_me) ) == -1){
        die("bind");
    }

    return s;
}