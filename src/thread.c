#include "thread.h"

ThreadConfig *buildThreadConfig(int rid, int socket, Queue *outputQueue, Queue *inputQueue){
    ThreadConfig *new = malloc(sizeof(*new));
    new->rid = rid;
    new->socket = socket;
    new->inputQueue = inputQueue;
    new->outputQueue = outputQueue;
    return new;
}

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
        Message *msg = NULL;
        memset(buffer,'\0', BUFFER);
        memset((char *) &si_other, 0, sizeof(si_other));
        msg = dequeue(arr->outputQueue);

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
        si_other.sin_port = htons(atoi(adress[1]));
        if (inet_aton(adress[0], &si_other.sin_addr) == 0) 
        {
            die("inet_aton()");
        }

        if (sendto(arr->socket, buffer, strlen(buffer), 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            die("sento");
        }
    }
}

void *receiver(void *config){
    struct sockaddr_in sin_other;
    int slen = sizeof(sin_other);
    ThreadConfig *arr = (ThreadConfig *)config;

    while(1){
        char buffer[BUFFER];

        if ((recvfrom(arr->socket, buffer, BUFFER, 0, (struct sockaddr *)&sin_other, &slen)) == -1)
        {
            die("recvfrom");
        }

        char **result = stringSplit(buffer, "|");
        Message *msg  = buildMessage((int)result[0][0] - 48, result[1], result[2], (void *)result[3], slen);

        enqueue(arr->inputQueue, msg);
    }
}

void *packetHandler (void *config) {
    ThreadConfig *arr = (ThreadConfig *)config;
    while(1){
        Message *msg = dequeue(arr->inputQueue); // retornando struct vazia
        char **adress = stringSplit(msg->destiny, ":");
        char port[6];

        snprintf(port, 6,"%d", 25000+arr->rid);

        if(!strcmp(adress[0], "127.0.0.1") && !strcmp(adress[1], port)){
            char **adress = stringSplit(msg->root, ":");
            printf("\n%d: %s\n", atoi(adress[1]) - 25000,(char *)msg->payload);
        } else{
            enqueue(arr->outputQueue, msg);
        }
    }
}

void *terminal (void *config) {
    // starting variables
    ThreadConfig *att = (ThreadConfig *)config;
    char buffer[BUFFER];
    // mapping links to a adj matrix
    int nlinks = countr();
    char rid = att->rid + 48;
    int **links = rlink(rid);

    // starting main loop
    do{
        int drouter;
        printf("\n------------------------- Router %d -------------------------\n", att->rid);
        printf("Neighborhood\n");
        for(int i = 0; i < nlinks; i++){
            if (i != att->rid - 1){
                printf("%d: ",i+1);
                for(int j = 0; j < nlinks; j++) if (links[i][j]) printf("%d ", links[i][j]);
                printf("\n");
            }
        }
        printf("Input Formart: ROUTER Message\n");
        printf("type '-1 q' to exit\n");
        printf("Message: ");
        scanf("%d", &drouter);
        getchar();
        fgets(buffer, BUFFER, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if(drouter != -1 && links[att->rid -1][drouter -1]){
            char root[16];
            char destiny[16];

            snprintf(root, 16,"127.0.0.1:%d", 25000 + att->rid);
            snprintf(destiny, 16,"127.0.0.1:%d", 25000 + drouter);

            enqueue(att->outputQueue, buildMessage(1,root, destiny, (void *)buffer, BUFFER));
        }else{
            if(drouter != -1){
                printf("Can't reach router %d\n", drouter);
            }
        }
    }while(strcmp(buffer, "q"));
}