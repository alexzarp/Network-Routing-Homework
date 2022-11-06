#ifndef MSG
#define MSG

typedef struct message{
    int type;
    char *root;
    char *destiny;
    int len;
    void *payload;
}Message;

Message *buildMessage(int type, char *root, char *destiny, void *payload, int len);
void displayMessage(Message *msg);
void freeMessage(Message *msg);

#endif /*MSG*/