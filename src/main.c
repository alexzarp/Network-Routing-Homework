#include "control.h"
#include "rof.h"
 
int main(int argc, char **argv) {
    pthread_t thrd0, thrd1, thrd2, thrd3, thrd4;

    // iniciando socket e filas
    int s = buildRouter(rrouter(argv[1][0]));
    Queue *output = buildQueue(QUEUESIZE);
    Queue *input = buildQueue(QUEUESIZE);

    // criando arquivos de configuração das threads
    ThreadConfig *mes_recv = buildThreadConfig((int)argv[1][0] - 48, s, NULL, input);
    ThreadConfig *mes_send = buildThreadConfig((int)argv[1][0] - 48, s, output, NULL);
    ThreadConfig *mes_pack = buildThreadConfig((int)argv[1][0] - 48, s, output, input);
    ThreadConfig *mes_term = buildThreadConfig((int)argv[1][0] - 48, s, output, NULL);
    ThreadConfig *mes_ping = buildThreadConfig((int)argv[1][0] - 48, s, output, NULL);

    // iniciando threads
    pthread_create(&thrd0, NULL, receiver, (void*) mes_recv);
    pthread_create(&thrd1, NULL, sender, (void*) mes_send);
    pthread_create(&thrd2, NULL, packetHandler, (void*) mes_pack);
    pthread_create(&thrd3, NULL, terminal, (void*) mes_term);
    pthread_create(&thrd4, NULL, ping, (void*) mes_ping);

    // join na thread de terminal
    pthread_join(thrd3, NULL);

    close(s);
    return 0;
}
