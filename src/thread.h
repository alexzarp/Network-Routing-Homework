#ifndef THRD
#define THRD

#include "control.h"
#include "list.h"

typedef struct status Status;
typedef struct queue Queue;

typedef struct {
    int rid;
    int socket;
    Status *srouter;
    int nrouters;
    Queue *outputQueue;
    Queue *inputQueue;
}ThreadConfig;

// cria o arquivo de configuração que será passado como argumento para as Threads
ThreadConfig *buildThreadConfig(int rid, int socket, Status *srouter, Queue *outputQueue, Queue *inputQueue);

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

// controla os timeout de estado dos roteadores
void *pong(void *config);

// envia o vetor distância para os vizinhos
void *gossip(void *config);

#endif /*THRD*/