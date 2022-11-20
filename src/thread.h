#ifndef THRD
#define THRD

#include "control.h"

typedef struct queue Queue;

typedef struct {
    int rid;
    int socket;
    int nrouters;
    Queue *outputQueue;
    Queue *inputQueue;
}ThreadConfig;

// cria o arquivo de configuração que será passado como argumento para as Threads
ThreadConfig *buildThreadConfig(int rid, int socket, Queue *outputQueue, Queue *inputQueue);

// retira mensagens da fila de saida e as envia
void *sender(void *config);

// retira mensagens da fila de entrada e as direciona para o stdout ou para fila de saida
void *packetHandler (void *config);

// espera chegada de mensagens no socket e as insere na fila de entrada  
void *receiver(void *config);

// recebe comandos e mensagens do usuário, as redireciona para fila de saída
void *terminal (void *config);

// envia mensagens de ping para os vizinhos
void *ping(void *config);

#endif /*THRD*/