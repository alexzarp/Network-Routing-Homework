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
    char *mes_recv = "receiver thread";
    char *mes_send = "sender thread";
    char *mes_pack = "packet_handler thread";
    char *mes_term = "terminal thread";
    int ret_recv, ret_send, ret_pack, ret_term;

    ret_recv = pthread_create(&thrd0, NULL, receiver, (void*) mes_recv);
    ret_send = pthread_create(&thrd1, NULL, sender, (void*) mes_send);
    ret_pack = pthread_create(&thrd2, NULL, packet_handler, (void*) mes_pack);
    ret_term = pthread_create(&thrd3, NULL, terminal, (void*) mes_term);

    // id roteador a ser inicializado
    int s = buildRouter(rrouter((int)argv[0]));

    pthread_join(thrd0, NULL);
    pthread_join(thrd1, NULL);
    pthread_join(thrd2, NULL);
    pthread_join(thrd3, NULL);

    close(s);
    return 0;
}


 
