#include "thread.h"
#include "list.h"

List *links = NULL;
pthread_mutex_t link_mutex = PTHREAD_MUTEX_INITIALIZER;

ThreadConfig *buildThreadConfig(int rid, int socket, Status *srouter, Queue *outputQueue, Queue *inputQueue){
    ThreadConfig *new = malloc(sizeof(*new));
    new->rid = rid;
    new->socket = socket;
    new->nrouters = countr();
    new->srouter = srouter;
    new->inputQueue = inputQueue;
    new->outputQueue = outputQueue;
    return new;
}

static char **stringSplit(char *payload, const char *sep){
    char **result = NULL;
    int size = 0;

    for(int i = 0; payload[i] != '\0'; i++){
        size += (int)payload[i] == (int)sep[0] ? 1 : 0; 
    }

    result = calloc(size+1, sizeof(char *));

    char *token = strtok_r(payload, sep, &payload);
    result[0] = token;
    int i = 1;

    while ((token = strtok_r(payload, sep, &payload))){
        result[i] = token;
        i++;
    }

    return result;
}

static char *replaceWord(const char* s, const char* oldW, const char* newW) { 
    char* result; 
    int i, cnt = 0; 
    int newWlen = strlen(newW); 
    int oldWlen = strlen(oldW); 
  
    // Counting the number of times old word 
    // occur in the string 
    for (i = 0; s[i] != '\0'; i++) { 
        if (strstr(&s[i], oldW) == &s[i]) { 
            cnt++; 
  
            // Jumping to index after the old word. 
            i += oldWlen - 1; 
        } 
    } 
  
    // Making new string of enough length 
    result = (char*)malloc(i + cnt * (newWlen - oldWlen) + 1); 
  
    i = 0; 
    while (*s) { 
        // compare the substring with the result 
        if (strstr(s, oldW) == s) { 
            strcpy(&result[i], newW); 
            i += newWlen; 
            s += oldWlen; 
        } 
        else
            result[i++] = *s++; 
    } 
  
    result[i] = '\0'; 
    return result; 
}

static int countChar(char *string, char caracter){
    int i = 0, count=0;
    while(string[i] != '\0'){
        if (string[i] == caracter) count++;
        i++;
    }
    return count;
}

static void resetLinks(const int self, const int neighbor){
    if(!links) return;

    pthread_mutex_lock(&link_mutex);

    List *dv = (List *)getList(links, self);
    removeList(dv, neighbor);
    removeList(links, neighbor);

    pthread_mutex_unlock(&link_mutex);
}

static void displayNeighborhood(int rid){
    void printer(int id, void *data){
        if(id == rid) return;
        Data value = *((Data *) data);
        value.cost != INF ? printf("%d: %d\n", id, value.cost) : printf("%d: %s\n", id, "INFITY"); 
    }
    printf("\n------------------------- Router %d -------------------------\n", rid);
    printf("Neighborhood\n");

    pthread_mutex_lock(&link_mutex);

    List *dv = (List *)getList(links, rid);
    walksList(dv, printer);

    pthread_mutex_unlock(&link_mutex);
}

static void bellmanFordBuilder(int rid, int grid, List *l, const char *vectord) {
    char temp_string[BUFFER];
    snprintf(temp_string, BUFFER, "%s", vectord);
    int len = countChar(temp_string, '-') + 1;
    char **tuples = stringSplit(temp_string, "-");

    for(int i = 0; i < len; i++) {
        tuples[i] = replaceWord(tuples[i], "(", "");
        tuples[i] = replaceWord(tuples[i], ")", "");
    }

    List *newVD = buildList(1);
    
    for (int i = 0; i < len; i++) {
        char **id_cost = stringSplit(tuples[i], ",");
        int idb = atoi(id_cost[0]);
        int costb = atoi(id_cost[1]);

        Data *dtemp = malloc(sizeof(Data));
        dtemp->cost = costb;
        dtemp->timeout = 5;
        dtemp->parent = INF;

        int bordererCost;
        
        Data *temp = getList(l, idb);
        if (temp) { // se ja existe
            if(temp->parent != -1){
                Data *gateway = getList(l, temp->parent);
                bordererCost = gateway ? gateway->cost : INF;
                
                if (costb && temp->cost > costb + bordererCost) {
                    temp->cost = costb + bordererCost;
                    temp->parent = grid;
                }
            }
        } else { // se nao existe
            Data *gateway = getList(l, grid);
            bordererCost = gateway ? gateway->cost : 0;

            Data *new = malloc(sizeof(Data));
            new->cost = costb + bordererCost;
            new->timeout = 5;
            new->parent = grid;
            addList(l, idb, (void *)new);   
        }
        
        addList(newVD, idb, (void *)dtemp);
    }
    List *griddv = (List *) getList(links, grid);
    if (griddv) removeList(links, grid);
    addList(links, grid, (void *)newVD); // lista
}

void *sender(void *config){
    struct sockaddr_in si_other;
    int slen = sizeof(si_other);
    ThreadConfig *arr = (ThreadConfig *)config;
    char buffer[BUFFER];

    while(1){
        Message *msg;
        memset(buffer,'\0', BUFFER);
        memset((char *) &si_other, 0, sizeof(si_other));
        msg = dequeue(arr->outputQueue);

        char type[2];
        snprintf(type, 2,"%d", msg->type);
        strcat(buffer, type);
        strcat(buffer, "|");
        strcat(buffer, msg->root);
        strcat(buffer, "|");
        strcat(buffer, msg->destiny);
        strcat(buffer, "|");
        strcat(buffer, (char *)msg->payload);
        strcat(buffer, "|");
        char ttl[10];
        snprintf(ttl, 10,"%d", msg->ttl);
        strcat(buffer, ttl);

        if(msg->middle){
            if(!strcmp((char *)msg->middle, "127.0.0.1")){
                free(msg);
                continue;
            }
            char **adress = stringSplit((char *)msg->middle, ":");
            si_other.sin_family = AF_INET;
            si_other.sin_port = htons(atoi(adress[1]));
            if (inet_aton(adress[0], &si_other.sin_addr) == 0) 
            {
                die("sender inet_aton()");
            }
        }else{
            if(!strcmp((char *)msg->destiny, "127.0.0.1")){
                free(msg);
                continue;
            }
            char **adress = stringSplit((char *)msg->destiny, ":");
            si_other.sin_family = AF_INET;
            si_other.sin_port = htons(atoi(adress[1]));
            if (inet_aton(adress[0], &si_other.sin_addr) == 0) 
            {
                die("sender inet_aton()");
            }
        }
        if (sendto(arr->socket, buffer, strlen(buffer), 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            die("sender sento");
        }
        free(msg);
    }
}

void *receiver(void *config){
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
        char **result = stringSplit(buffer, "|");
        Message *msg  = buildMessage((int)result[0][0] - 48, result[1], NULL, result[2], (void *)result[3], slen, atoi(result[4]));
        enqueue(arr->inputQueue, msg);
    }
}

void *packetHandler (void *config) {
    ThreadConfig *arr = (ThreadConfig *)config;

    pthread_mutex_lock(&link_mutex);
    List *myrouter = getList(links, arr->rid);
    pthread_mutex_unlock(&link_mutex);

    while(1){
        char temp_adress[16];
        char port[6];
        Message *msg = dequeue(arr->inputQueue);

        if (!msg->ttl) {
            pthread_mutex_lock(&link_mutex);
            freeList(links);
            links = rlink(arr->rid);
            pthread_mutex_unlock(&link_mutex);
        }

        snprintf(temp_adress, 16,"%s", (char *)msg->destiny);
        char **adress = stringSplit(temp_adress, ":");

        snprintf(port, 6,"%d", 25000+arr->rid);

        if(!strcmp(adress[0], "127.0.0.1") && !strcmp(adress[1], port)){
            if (msg->type){
                char **adress = stringSplit(msg->root, ":");
                printf("\n%d: %s\n", atoi(adress[1]) - 25000,(char *)msg->payload);
            } else {
                char **control_msg = stringSplit((char *)msg->payload, " ");

                if (!strcmp(control_msg[0], "gossip")){
                    pthread_mutex_lock(&link_mutex);
                    Data *neighborhood = (Data *)getList(myrouter, atoi(control_msg[1]));
                    if(neighborhood){
                        neighborhood->timeout = 5;
                    }
                    bellmanFordBuilder(arr->rid, atoi(control_msg[1]), myrouter, control_msg[2]);
                    pthread_mutex_unlock(&link_mutex);
                }
            }
        } else{
            pthread_mutex_lock(&link_mutex);
            char *middle = NULL;
            Data *neighborhood = (Data *)getList(myrouter, atoi(adress[1]) - 25000);
            if (neighborhood && neighborhood->parent != arr->rid){
                char *middle = calloc(16, sizeof(char));
                snprintf(middle, 16,"127.0.0.1:%d", 25000 + neighborhood->parent);
            }
            msg->middle = middle;
            pthread_mutex_unlock(&link_mutex);
            msg->ttl--;
            enqueue(arr->outputQueue, msg);
        }
    }
}

void *terminal (void *config) {
    ThreadConfig *att = (ThreadConfig *)config;
    char buffer[BUFFER];

    char rid = att->rid + 48;

    pthread_mutex_lock(&link_mutex);
    links = rlink(rid);
    List *dv = (List *)getList(links, att->rid);
    pthread_mutex_unlock(&link_mutex);

    do{
        int drouter;
        displayNeighborhood(att->rid);
        printf("Input Formart: ROUTER Message\n");
        printf("type '-1 q' to exit\n");
        printf("Message: ");
        scanf("%d", &drouter);
        getchar();
        fgets(buffer, BUFFER, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';


        pthread_mutex_lock(&link_mutex);

        Data *dvrouter = (Data *)getList(dv, drouter);
        if(drouter != -1 && dvrouter){
            char *root = calloc(16, sizeof(char));
            char *destiny = calloc(16, sizeof(char));
            char *middle = NULL;

            if(dvrouter->parent != att->rid){
                middle = calloc(16, sizeof(char));
                snprintf(middle, 16,"127.0.0.1:%d", 25000 + dvrouter->parent);
            }
            
            pthread_mutex_unlock(&link_mutex);

            snprintf(root, 16,"127.0.0.1:%d", 25000 + att->rid);
            snprintf(destiny, 16,"127.0.0.1:%d", 25000 + drouter);
            enqueue(att->outputQueue, buildMessage(1,root, middle, destiny, (void *)buffer, BUFFER, TTL));
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
    ThreadConfig *att = (ThreadConfig *)config;

    while(1){
        pthread_mutex_lock(&link_mutex);
        if(links){
            void sendPing(int id, void *data){
                if(id == att->rid) return;
                char *root = malloc(sizeof(char) * 16);
                char *destiny = malloc(sizeof(char) * 16);
                char *ping = malloc(sizeof(char) * 7);

                //printf("\nPing: Build ping message\n");
				snprintf(root, 16,"127.0.0.1:%d", 25000 + att->rid);
				snprintf(destiny, 16,"127.0.0.1:%d", 25000 + id);
                snprintf(ping, 7,"ping %d", att->rid);
				Message *msg = buildMessage(0, root, NULL, destiny, (void *)ping, 7, TTL);

                enqueue(att->outputQueue, msg);
            }
            List *dv = (List *) getList(links, att->rid);
            walksList(dv, sendPing);
        }

        pthread_mutex_unlock(&link_mutex);
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
    typedef struct{
        char *string;
        int size;
        int current;
    }CompString;

    ThreadConfig *att = (ThreadConfig *)config;
    while(1){
        pthread_mutex_lock(&link_mutex);

        if(links){
            int gtam = BUFFER + 10;

            char *temp = calloc(BUFFER, sizeof(char));

            CompString *aux = malloc(sizeof(CompString));
            aux->current = 0;
            aux->size = BUFFER;
            aux->string = temp;

            // distance vector
            List *dv = (List *) getList(links, att->rid);

            void buildGossip(void *acumulator, int id, void *data){
                CompString *cs = (CompString *) acumulator;
                Data value = *((Data *) data);
                cs->current += snprintf(&cs->string[cs->current], (BUFFER)-cs->current, "(%d,%d)-", id, value.cost);
            }

            reduceList(dv, (void *)aux, buildGossip);
            aux->string[aux->current-1] = '\0';
            
            void sendGossip(int id, void *data){
                if (id == att->rid) return;
                char *root = calloc(16, sizeof(char));
                char *destiny = calloc(16, sizeof(char));
                char *gossip = calloc(gtam, sizeof(char));

                snprintf(root, 16,"127.0.0.1:%d", 25000 + att->rid);
                snprintf(destiny, 16,"127.0.0.1:%d", 25000 + id);
                snprintf(gossip, gtam,"gossip %d %s", att->rid, aux->string);
                Message *msg = buildMessage(0, root, NULL,destiny,(void *)gossip, BUFFER, TTL);
                enqueue(att->outputQueue, msg);
            }
            walksList(dv, sendGossip);
        }
        pthread_mutex_unlock(&link_mutex);
        sleep(TIMEOUT * 2);
    }
}

void *killer(void *config) {
    ThreadConfig *att = (ThreadConfig *)config;

    List *dv = (List *) getList(links, att->rid);
    while (1){
        pthread_mutex_lock(&link_mutex);
        
        void decrement(int id, void *data){
            Data *aux = (Data *) data;
            if(att->rid == id || aux->parent != att->rid) return;
            aux->timeout--;
        }

        walksList(dv, decrement);

        void remover(int id, void *data) {
            if(att->rid == id) return;
            Data *aux = (Data *) data;
            if (!aux->timeout) {
                setStatus(att->srouter, id, 0);
                removeList(dv, id);
                removeList(links, id);
            }
        }

        walksList(dv, remover);

        pthread_mutex_unlock(&link_mutex);
        sleep(TIMEOUT);
    }
}