#include "control.h"
#include "rof.h"
 
int main(int argc, char **argv) {
    pthread_t tpid[8];
    ThreadConfig *configs[8];

    // iniciando socket e filas
    int s = buildRouter(rrouter(argv[1][0]));
    Queue *output = buildQueue(QUEUESIZE);
    Queue *input = buildQueue(QUEUESIZE);
    Status *srouter = buildStatus((int)argv[1][0] - 48);

    // criando arquivos de configuração das threads
    configs[0] = buildThreadConfig((int)argv[1][0] - 48, s, srouter, NULL, input);
    configs[1] = buildThreadConfig((int)argv[1][0] - 48, s, srouter, output, NULL);
    configs[2] = buildThreadConfig((int)argv[1][0] - 48, s, srouter, output, input);
    configs[3] = buildThreadConfig((int)argv[1][0] - 48, s, srouter, output, NULL);
    configs[4] = buildThreadConfig((int)argv[1][0] - 48, s, srouter, output, NULL);
    configs[5] = buildThreadConfig((int)argv[1][0] - 48, s, srouter, NULL, NULL);
    configs[6] = buildThreadConfig((int)argv[1][0] - 48, s, srouter, output, NULL);
    configs[7] = buildThreadConfig((int)argv[1][0] - 48, s, srouter, output, NULL);

    // iniciando threads
    pthread_create(&tpid[3], NULL, terminal, (void*) configs[3]);
    sleep(1);
    pthread_create(&tpid[0], NULL, receiver, (void*) configs[0]);
    pthread_create(&tpid[1], NULL, sender, (void*) configs[1]);
    pthread_create(&tpid[2], NULL, packetHandler, (void*) configs[2]);
    //pthread_create(&tpid[4], NULL, ping, (void*) configs[4]);
    //pthread_create(&tpid[5], NULL, pong, (void*) configs[5]);
    pthread_create(&tpid[6], NULL, gossip, (void*) configs[6]);
    pthread_create(&tpid[7], NULL, killer, (void*) configs[7]);

    // join na thread de terminal
    pthread_join(tpid[3], NULL);

    close(s);
    return 0;
}
