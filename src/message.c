#include "control.h"
#include "message.h"

Message *buildMessage(int type, char *root, char *destiny, void *payload, int len){
    Message *msg = malloc(sizeof(*msg));
    msg->root = root;
    msg->destiny = destiny;
    msg->payload = payload;
    msg->type = type;
    msg->len = len;
    return msg;
}

void displayMessage(Message *msg){
    printf("\n------------------------------\n");
    printf("HEADER\n");
    printf("\tMessage Type: %d\n", msg->type);
    printf("\tRoot: %s\n", msg->root);
    printf("\tDestiny: %s\n", msg->destiny);
    printf("PAYLOAD\n\t%s\n", (char *)msg->payload);
    printf("------------------------------\n");
}

void freeMessage(Message *msg){
    free(msg->destiny);
    free(msg->root);
    free(msg->payload);
    free(msg);
}