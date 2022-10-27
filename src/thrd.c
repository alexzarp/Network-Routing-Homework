#include "control.h"

static char **stringSplit(char *payload, char *sep){
    char **result = NULL;
    int size = 0;

    for(int i = 0; payload[i] != '\0'; i++){
        size += (int)payload[i] == (int)sep[0] ? 1 : 0; 
    }

    result = calloc(size+1, sizeof(char *));

    char *token = strtok(payload, sep);
    result[0] = token;
    int i = 1;

    while(token != NULL){
        token = strtok(NULL, sep);
        result[i] = token;
        i++;
    }

    return result;
}

void *sender(void *config){
    struct sockaddr_in si_other;
    int slen = sizeof(si_other);
    ThreadConfig *arr = (ThreadConfig *)config;
    char buffer[BUFFER];

    while(1){
        memset(buffer,'\0', BUFFER);
        memset((char *) &si_other, 0, sizeof(si_other));
        Message *msg = dequeue(arr->outputQueue);

        char type[2];
        sprintf(type, "%d", msg->type);
        strcat(buffer, type);
        strcat(buffer, "|");
        strcat(buffer, msg->root);
        strcat(buffer, "|");
        strcat(buffer, msg->destiny);
        strcat(buffer, "|");
        strcat(buffer, (char *)msg->payload);

        char **adress = stringSplit((char *)msg->destiny, ":");
        si_other.sin_family = AF_INET;
        si_other.sin_port = htons(atoi(adress[2]));
        if (inet_aton(adress[0], &si_other.sin_addr) == 0) 
        {
            fprintf(stderr, "inet_aton() failed\n");
            exit(2);
        }

        if (sendto(arr->socket, buffer, strlen(buffer), 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            exit(2);
        }
    }
}

void *receiver(void *config){
    struct sockaddr_in sin_other;
    int slen = sizeof(sin_other);
    ThreadConfig *arr = (ThreadConfig *)config;
    char buffer[BUFFER];

    while(1){
        memset(buffer,'\0', BUFFER);

        if ((recvfrom(arr->socket, buffer, BUFFER, 0, (struct sockaddr *)&sin_other, &slen)) == -1)
        {
            exit(2);
        }

        char **result = stringSplit(buffer, "|");

        enqueue(arr->inputQueue, buildMessage((int)result[0][0] - 48, result[1], result[2], (void *)result[3], slen));
    }
    pthread_exit(NULL);
}

void *packet_handler (void *config) {
    ThreadConfig *att = (ThreadConfig *)config;
    while(1){
        Message *msg = dequeue(att->inputQueue);
        char **adress = stringSplit((char *)msg->destiny, ":");

        if(htonl(adress[0]) == att->addrMe->sin_addr.s_addr && htons(adress[1]) == att->addrMe->sin_port){
            char **output = stringSplit((char *)msg->payload, "|");
            printf("%s", output[3]);
        } else{
            enqueue(att->outputQueue, msg);
        }
    }
}

void *terminal (void *config) { // ThreadConfig.socket = router id;
    ThreadConfig *att = (ThreadConfig *)config;
    char *buffer[BUFFER];

    do{
        printf("\n------------------------- Router %d -------------------------\n", att->socket);
        printf("Input Formart: IP:PORT Message\n");
        printf("type q to exit\n");
        printf("Message: ");
        fgets(buffer, BUFFER, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
    }while(buffer[0] != 'q');
}