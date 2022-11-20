#include "control.h"
#include "rof.h"
 
int main(int argc, char **argv) {
    pthread_t tpid[6];

    // iniciando socket e filas
    int s = buildRouter(rrouter(argv[1][0]));
    Queue *output = buildQueue(QUEUESIZE);
    Queue *input = buildQueue(QUEUESIZE);
    Status *srouter = buildStatus((int)argv[1][0] - 48, countr());

    // criando arquivos de configuração das threads
    ThreadConfig *mes_recv = buildThreadConfig((int)argv[1][0] - 48, s, srouter, NULL, input);
    ThreadConfig *mes_send = buildThreadConfig((int)argv[1][0] - 48, s, srouter, output, NULL);
    ThreadConfig *mes_pack = buildThreadConfig((int)argv[1][0] - 48, s, srouter, output, input);
    ThreadConfig *mes_term = buildThreadConfig((int)argv[1][0] - 48, s, srouter, output, NULL);
    ThreadConfig *mes_ping = buildThreadConfig((int)argv[1][0] - 48, s, srouter, output, NULL);
    ThreadConfig *mes_pong = buildThreadConfig((int)argv[1][0] - 48, s, srouter, NULL, NULL);

    // iniciando threads
    pthread_create(&tpid[0], NULL, receiver, (void*) mes_recv);
    pthread_create(&tpid[1], NULL, sender, (void*) mes_send);
    pthread_create(&tpid[2], NULL, packetHandler, (void*) mes_pack);
    pthread_create(&tpid[3], NULL, terminal, (void*) mes_term);
    pthread_create(&tpid[4], NULL, ping, (void*) mes_ping);
    pthread_create(&tpid[5], NULL, pong, (void*) mes_pong);

    // join na thread de terminal
    pthread_join(tpid[3], NULL);

    close(s);
    return 0;
}
