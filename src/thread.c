#include "thread.h"

int **links = NULL;
int *parents = NULL;
pthread_mutex_t link_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t parents_mutex = PTHREAD_MUTEX_INITIALIZER;

ThreadConfig *buildThreadConfig(int rid, int socket, Status *srouter,Queue *outputQueue, Queue *inputQueue){
    ThreadConfig *new = malloc(sizeof(*new));
    new->rid = rid;
    new->socket = socket;
    new->nrouters = countr();
    new->srouter = srouter;
    new->inputQueue = inputQueue;
    new->outputQueue = outputQueue;
    return new;
}

static char **stringSplit(char *payload,const char *sep){
    char **result = NULL;
    int size = 0;

    for(int i = 0; payload[i] != '\0'; i++){
        size += (int)payload[i] == (int)sep[0] ? 1 : 0; 
    }

    result = calloc(size+1, sizeof(char *));

    char *token = strtok_r(payload, sep, &payload);
    result[0] = token;
    int i = 1;

    while ((token = strtok_r(payload, " ", &payload))){
        result[i] = token;
        i++;
    }

    return result;
}

static void resetLinks(const int rid, const int size){
    if(!links) return;

    pthread_mutex_lock(&link_mutex);

    for(int i = 0; i < size; i++)
        for(int j = 0; j < size; j++)
            if(!links[i][rid-1] && !links[rid-1][j]) links[i][j] = 0;

    pthread_mutex_unlock(&link_mutex);
}

void *sender(void *config){
    //printf("\nSender: Load arguments\n");
    struct sockaddr_in si_other;
    int slen = sizeof(si_other);
    ThreadConfig *arr = (ThreadConfig *)config;
    char buffer[BUFFER];

    while(1){
        //printf("\nSender: Get message from output queue\n");
        Message *msg;
        memset(buffer,'\0', BUFFER);
        memset((char *) &si_other, 0, sizeof(si_other));
        msg = dequeue(arr->outputQueue);

        //printf("\nSender: Map message to string\n");
        char type[2];
        snprintf(type, 2,"%d", msg->type);
        strcat(buffer, type);
        strcat(buffer, "|");
        strcat(buffer, msg->root);
        strcat(buffer, "|");
        strcat(buffer, msg->destiny);
        strcat(buffer, "|");
        strcat(buffer, (char *)msg->payload);

        //printf("\nSender: Load destiny socket informations\n");
        char **adress = stringSplit((char *)msg->destiny, ":");
        si_other.sin_family = AF_INET;
        si_other.sin_port = htons(atoi(adress[1]));
        if (inet_aton(adress[0], &si_other.sin_addr) == 0) 
        {
            die("sender inet_aton()");
        }
        //printf("\nSender: Send message\n");
        if (sendto(arr->socket, buffer, strlen(buffer), 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            die("sender sento");
        }
        free(msg);
    }
}

void *receiver(void *config){
    //printf("\nReceiver: Load arguments\n");
    struct sockaddr_in sin_other;
    int slen = sizeof(sin_other);
    ThreadConfig *arr = (ThreadConfig *)config;

    while(1){
        char *buffer = malloc(sizeof(char) * BUFFER);
        //printf("\nReceiver: Wait new messages\n");
        if ((recvfrom(arr->socket, buffer, BUFFER, 0, (struct sockaddr *)&sin_other, &slen)) == -1)
        {
            die("recvfrom");
        }
        //printf("\nReceiver: Load incomming message\n");
        char **result = stringSplit(buffer, "|");
        Message *msg  = buildMessage((int)result[0][0] - 48, result[1], result[2], (void *)result[3], slen);
        displayMessage(msg);
        //printf("\nReceiver: Enqueue message in input queue\n");
        enqueue(arr->inputQueue, msg);
    }
}

void *packetHandler (void *config) {
    //printf("\nPH: Load arguments\n");
    ThreadConfig *arr = (ThreadConfig *)config;
    while(1){
        //printf("\nPH: Get message from input queue\n");
        Message *msg = dequeue(arr->inputQueue);
        char **adress = stringSplit(msg->destiny, ":");
        char port[6];

        snprintf(port, 6,"%d", 25000+arr->rid);

        //printf("\nPH: Check if current route is his destiny\n");
        if(!strcmp(adress[0], "127.0.0.1") && !strcmp(adress[1], port) && msg->type){
            char **adress = stringSplit(msg->root, ":");
            printf("\n%d: %s\n", atoi(adress[1]) - 25000,(char *)msg->payload);
        }else if (!msg->type){
            char **control_msg = stringSplit((char *)msg->payload, " ");
            if (!strcmp(control_msg[0], "ping")){
                setStatus(arr->srouter, atoi(control_msg[1]), 1);
            }

            if (!strcmp(control_msg[0], "gossip")){
                printf("\n%s: %s\n", control_msg[1], control_msg[2]);
            }
        } else{
            //printf("\nPH: Enqueue msg in output queue\n");
            enqueue(arr->outputQueue, msg);
        }
    }
}

void *terminal (void *config) {
    //printf("\nTerminal: Load variables\n");
    // starting variables
    ThreadConfig *att = (ThreadConfig *)config;
    char buffer[BUFFER];
    // mapping links to a adj matrix
    char rid = att->rid + 48;

    //printf("\nTerminal: Init links matrix\n");
    pthread_mutex_lock(&link_mutex);
    links = rlink(rid);
    pthread_mutex_unlock(&link_mutex);

    pthread_mutex_lock(&parents_mutex);
    parents = calloc(att->nrouters, sizeof(int));
    pthread_mutex_unlock(&parents_mutex);

    // starting main loop
    do{
        int drouter;
        printf("\n------------------------- Router %d -------------------------\n", att->rid);
        printf("Neighborhood\n");
        pthread_mutex_lock(&link_mutex);
        for(int i = 0; i < att->nrouters; i++){
            if (i != att->rid - 1){
                printf("%d: ",i+1);
                for(int j = 0; j < att->nrouters; j++) if (links[i][j]) printf("%d ", links[i][j]);
                printf("\n");
            }
        }
        pthread_mutex_unlock(&link_mutex);
        printf("Input Formart: ROUTER Message\n");
        printf("type '-1 q' to exit\n");
        printf("Message: ");
        scanf("%d", &drouter);
        getchar();
        fgets(buffer, BUFFER, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        pthread_mutex_lock(&link_mutex);
        //printf("\nTerminal: Check if selected route is reacheble\n");
        if(drouter != -1 && links[att->rid -1][drouter -1]){
            //printf("\nTerminal: Load message\n");
            pthread_mutex_unlock(&link_mutex);
            char root[16];
            char destiny[16];

            snprintf(root, 16,"127.0.0.1:%d", 25000 + att->rid);
            snprintf(destiny, 16,"127.0.0.1:%d", 25000 + drouter);
            //printf("\nTerminal: Enqueue message to output queue\n");
            enqueue(att->outputQueue, buildMessage(1,root, destiny, (void *)buffer, BUFFER));
        }else{
            pthread_mutex_unlock(&link_mutex);
            if(drouter != -1){
                printf("Can't reach router %d\n", drouter);
            }
        }

        if(drouter == -1 && !strcmp(buffer, "lstatus")){
            displayStatus(att->srouter, att->rid);
        }
    }while(strcmp(buffer, "q"));
}

void *ping(void *config){
    //printf("\nPing: Load Configs and Couting Router in Network\n");
    ThreadConfig *att = (ThreadConfig *)config;

    while(1){
        //printf("\nPing: Lock link mutex\n");
        pthread_mutex_lock(&link_mutex);
        //printf("\nPing: Check if link matrix is null\n");
        if(links){
            //printf("\nPing: Find Neibors\n");
            for(int i = 0; i < att->nrouters; i++){
                if(links[att->rid-1][i]){
                    char *root = malloc(sizeof(char) * 16);
                    char *destiny = malloc(sizeof(char) * 16);
                    char ping[7];

                    //printf("\nPing: Build ping message\n");
					snprintf(root, 16,"127.0.0.1:%d", 25000 + att->rid);
					snprintf(destiny, 16,"127.0.0.1:%d", 25001 + i);
                    snprintf(ping, 7,"ping %d", att->rid);
					Message *msg = buildMessage(0, root, destiny, (void *)ping, 7);

                    //printf("\nPing: Enqueue ping message in ouput queue\n");

                    enqueue(att->outputQueue, msg);
                }
            }
        }
        //printf("\nPing: Unlock link mutex\n");
        pthread_mutex_unlock(&link_mutex);
        //printf("\nPing: Sleep for 5 seconds\n");
		sleep(TIMEOUT);
    }
}

void *pong(void *config){
    ThreadConfig *att = (ThreadConfig *)config;

    while(1){
        for(int i = 0; i < att->nrouters; i++){
            if(att->rid != i + 1){
                setStatus(att->srouter, i + 1, 0);
                resetLinks(att->rid, att->nrouters);
            }
        }
        sleep(TIMEOUT * 3);
    }
}

void *gossip(void *config){
    ThreadConfig *att = (ThreadConfig *)config;
    int ttam = att->nrouters * 4;
    int gtam = (att->nrouters * ttam) + 10;
    while(1){
        pthread_mutex_lock(&link_mutex);
        //printf("\nGossip: Check if link matrix is null\n");
        if(links){
            //printf("\nGossip: Find Neibors\n");
            for(int i = 0; i < att->nrouters; i++){
                if(links[att->rid-1][i]){
                    char *root = calloc(16, sizeof(char));
                    char *destiny = calloc(16, sizeof(char));
                    char *gossip = calloc(gtam, sizeof(char));
                    char temp[ttam];
                    int index = 0;

                    for(int j = 0; j < att->nrouters; j++){
                        index += snprintf(&temp[index], (ttam)-index, "%d-", links[att->rid-1][j]);
                    }

                    //printf("\nGossip: Build gossip message\n");
					snprintf(root, 16,"127.0.0.1:%d", 25000 + att->rid);
					snprintf(destiny, 16,"127.0.0.1:%d", 25001 + i);
                    snprintf(gossip, gtam,"gossip %d %s", att->rid, temp);
					Message *msg = buildMessage(0, root, destiny, (void *)gossip, gtam);

                    //printf("\nGossip: Enqueue gossip message in ouput queue\n");

                    enqueue(att->outputQueue, msg);
                }
            }
        }
        //printf("\nPing: Unlock link mutex\n");
        pthread_mutex_unlock(&link_mutex);
        sleep(TIMEOUT * 2);
    }
}