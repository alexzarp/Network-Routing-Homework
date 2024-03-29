#ifndef CONTROL
#define CONTROL

#include <pthread.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "rof.h"
#include "message.h"
#include "thread.h"
#include "status.h"

#define BUFFER 100 // tamanho da mensagem
#define QUEUESIZE 10 // tamanho da fila
#define TIMEOUT 5 // timeout para o ping
#define INF (int)2147483647 // valor infinito
#define TTL 10

typedef struct queue Queue;

typedef struct data{
    int cost;
    int timeout;
    int parent;
}Data;

// Queues Functions
Queue *buildQueue(int buffer); // cria uma fila de tamanho n
void enqueue(Queue *queue, Message *message); // insere uma mensagem na fila
Message *dequeue(Queue *queue); // remove uma mensagem da fila
void freeQueue(Queue *queue); // limpa os dados da fila

//Router Functions
int buildRouter (Router *router); // lê os arquivos de configuração e retorna o socket de comunicação
void die(char *s); // finaliza e programa e imprime um erro

#endif /*CONTROL*/