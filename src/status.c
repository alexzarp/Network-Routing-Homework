#include "status.h"

struct status{
    int size;
    int *sarray;
    pthread_mutex_t *mutex;
};

Status *buildStatus(int rid, int size){
    Status *new = malloc(sizeof(*new));
    new->size = size;
    new->sarray = calloc(size, sizeof(int));
    new->mutex = malloc(sizeof(pthread_mutex_t));

    new->sarray[rid - 1] = 1;
    pthread_mutex_init(new->mutex, NULL);

    return new;
}

int getStatus(Status *s, int rid){
    if (rid >= s->size || rid < 0){
        return -1;
    }

    int value;

    pthread_mutex_lock(s->mutex);
    value = s->sarray[rid - 1];
    pthread_mutex_unlock(s->mutex);

    return value;
}

void setStatus(Status *s, int rid, int value){
    if (rid >= s->size || rid < 0 || value > 1){
        return;
    }

    pthread_mutex_lock(s->mutex);
    s->sarray[rid-1] = value;
    pthread_mutex_unlock(s->mutex);
}

void freeStatus(Status *s){
    pthread_mutex_lock(s->mutex);
    free(s->sarray);
    pthread_mutex_unlock(s->mutex);
    pthread_mutex_destroy(s->mutex);
}