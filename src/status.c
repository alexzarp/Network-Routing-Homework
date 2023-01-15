#include "status.h"
#include "list.h"

struct status{
    List *slist;
    pthread_mutex_t *mutex;
};

Status *buildStatus(int rid){
    Status *new = malloc(sizeof(*new));
    new->slist = buildList(1);
    new->mutex = malloc(sizeof(pthread_mutex_t));
    int *data = malloc(sizeof(int));
    *data = 1;

    addList(new->slist, rid, (void *)data);
    pthread_mutex_init(new->mutex, NULL);

    return new;
}

int getStatus(Status *s, int rid){
    if (rid < 0) return -1;

    pthread_mutex_lock(s->mutex);
    int *value = (int *)getList(s->slist, rid);
    pthread_mutex_unlock(s->mutex);

    return value ? *value : -1;
}

void setStatus(Status *s, int rid, int value){
    if (rid < 0 || value > 1){
        return;
    }

    pthread_mutex_lock(s->mutex);
    int *current = (int *)getList(s->slist, rid);
    if(current){
        *current = value;
    }else{
        int *data = malloc(sizeof(int));
        *data = value;
        addList(s->slist, rid, (void *)data);
    }
    pthread_mutex_unlock(s->mutex);
}

void displayStatus(Status *s, int rid){
    void printStatus(int id, void *data){
        if (id == rid) return;
        int value = *((int *) data);
        printf("%d: %s\n", id, value ? "Online" : "Offline");
    }
    printf("\n------------------------- Routers Status -------------------------\n");
    pthread_mutex_lock(s->mutex);
    walksList(s->slist, printStatus);
    pthread_mutex_unlock(s->mutex);
}

void freeStatus(Status *s){
    pthread_mutex_lock(s->mutex);
    freeList(s->slist);
    pthread_mutex_unlock(s->mutex);
    pthread_mutex_destroy(s->mutex);
}