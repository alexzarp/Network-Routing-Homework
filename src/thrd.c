#include "control.h"
#include "rof.h"

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

    printf("Sender Start\n");

    while(1){
        memset(buffer,'\0', BUFFER);
        memset((char *) &si_other, 0, sizeof(si_other));
        printf("Sender: Get Message FROM QOUT\n");
        Message *msg = dequeue(arr->outputQueue);

        printf("Sender: Marshal Message in string\n");
        char type[2];
        sprintf(type, "%d", msg->type);
        strcat(buffer, type);
        strcat(buffer, "|");
        strcat(buffer, msg->root);
        strcat(buffer, "|");
        strcat(buffer, msg->destiny);
        strcat(buffer, "|");
        strcat(buffer, (char *)msg->payload);

        printf("Sender: Start Destiny Buffer\n");

        char **adress = stringSplit((char *)msg->destiny, ":");
        si_other.sin_family = AF_INET;
        si_other.sin_port = htons(atoi(adress[2]));
        if (inet_aton(adress[0], &si_other.sin_addr) == 0) 
        {
            fprintf(stderr, "inet_aton() failed\n");
            exit(2);
        }

        printf("Sender: Send message\n");

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

    printf("Receiver Start\n");

    while(1){
        memset(buffer,'\0', BUFFER);

        printf("Receiver: Receiving messages\n");

        if ((recvfrom(arr->socket, buffer, BUFFER, 0, (struct sockaddr *)&sin_other, &slen)) == -1)
        {
            exit(2);
        }

        printf("Receiver: Mapping message to struct\n");

        char **result = stringSplit(buffer, "|");

        printf("Receiver: Enqueue message in QIN\n");

        enqueue(arr->inputQueue, buildMessage((int)result[0][0] - 48, result[1], result[2], (void *)result[3], slen));
    }
    pthread_exit(NULL);
}

void *packet_handler (void *config) {
    ThreadConfig *att = (ThreadConfig *)config;
    printf("PH Start\n");
    while(1){
        printf("PH: Get Message from QIN\n");
        Message *msg = dequeue(att->inputQueue);
        char **adress = stringSplit((char *)msg->destiny, ":");
        char port[6];

        sprintf(port, "%d", 25000+att->rid);

        printf("PH: Redirecting Message from destinaction\n");

        if(!strcmp(adress[0], "127.0.0.1") && strcmp(adress[1], port)){
            char **output = stringSplit((char *)msg->payload, "|");
            printf("%s", output[3]);
        } else{
            enqueue(att->outputQueue, msg);
        }
    }
}

void *terminal (void *config) {
    ThreadConfig *att = (ThreadConfig *)config;
    printf("Starting msg buffer\n");
    char buffer[BUFFER];
    char rid = att->rid + 48;
    printf("RID is %c\n", rid);
    int **links = rlink(rid);

    printf("Terminal Start\n");

    do{
        int drouter;
        printf("\n------------------------- Router %d -------------------------\n", att->rid);
        printf("Input Formart: ROUTER Message\n");
        printf("type q to exit\n");
        printf("Message: ");
        scanf("%d", &drouter);
        getchar();
        fgets(buffer, BUFFER, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if(links[att->rid][drouter]){
            char root[15];
            char destiny[15];

            sprintf(root, "127.0.0.1:%d", 25000 + att->rid);
            sprintf(destiny, "127.0.0.1:%d", 25000 + drouter);
            
            enqueue(att->outputQueue, buildMessage(1,root, destiny, (void *)buffer, BUFFER));
        }else{
            printf("Can't reach router %d\n", drouter);
        }
    }while(strcmp(buffer, "q"));
}