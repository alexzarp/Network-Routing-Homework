#include "control.h"
#include "rof.h"

#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
 
int main(int argc, char **argv) {
    pthread_t thrd0, thrd1, thrd2, thrd3;

    int s = buildRouter(rrouter(argv[1][0]));
    Queue *output = buildQueue(QUEUESIZE);
    Queue *input = buildQueue(QUEUESIZE);

    ThreadConfig *mes_recv = buildThreadConfig((int)argv[1][0] - 48, s, input, NULL);
    ThreadConfig *mes_send = buildThreadConfig((int)argv[1][0] - 48, s, NULL, output);
    ThreadConfig *mes_pack = buildThreadConfig((int)argv[1][0] - 48, s, input, output);
    ThreadConfig *mes_term = buildThreadConfig((int)argv[1][0] - 48, s, NULL, output);

    pthread_create(&thrd0, NULL, receiver, (void*) mes_recv);
    pthread_create(&thrd1, NULL, sender, (void*) mes_send);
    pthread_create(&thrd2, NULL, packet_handler, (void*) mes_pack);
    pthread_create(&thrd3, NULL, terminal, (void*) mes_term);

    pthread_join(thrd3, NULL);

    close(s);
    return 0;
}


 
