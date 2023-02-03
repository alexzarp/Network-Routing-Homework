#ifndef MSG
#define MSG

typedef struct message{
    int type; // tipo: 1 - Mensagem | 0 - Controle
    char *root; // endereço ip da fonte]
    char *middle; // endereço ip do gateway mais próximo
    char *destiny; // endereço ip do destino
    int len; // tamanho da mensagem
    void *payload; // conteúdo da mensagem
    int ttl; // numero máximo de saltos de um pacote
}Message;

// cria uma mensagem
Message *buildMessage(int type, char *root, char *middle, char *destiny, void *payload, int len, int ttl);
// print de mensagem para debug
void displayMessage(Message *msg);
// libera memória alocada para mensagem
void freeMessage(Message *msg);

#endif /*MSG*/