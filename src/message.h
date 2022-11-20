#ifndef MSG
#define MSG

typedef struct message{
    int type; // tipo: 1 - Mensagem | 0 - Controle
    char *root; // endereço ip da fonte
    char *destiny; // endereço ip do destino
    int len; // tamanho da mensagem
    void *payload; // conteúdo da mensagem
}Message;

// cria uma mensagem
Message *buildMessage(int type, char *root, char *destiny, void *payload, int len);
// print de mensagem para debug
void displayMessage(Message *msg);
// libera memória alocada para mensagem
void freeMessage(Message *msg);

#endif /*MSG*/