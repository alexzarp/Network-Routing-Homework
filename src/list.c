#include "list.h"

typedef struct node{
    int id;
    void *data;
    struct node *next;
    struct node *prev;
}Node;

struct list{
    int type;
    Node *head;
    Node *tail;
};

List *buildList(int type){
    List *new = malloc(sizeof(*new));
    new->type = type;
    new->head = NULL;
    new->head = NULL;
}

void addList(List *l, void *data){
    switch(l->type){
        case 0:
            Node *new = malloc(sizeof(*new));
            new->id = l->tail ? l->tail->id + 1 : 0;
            new->data = buildList(l->type + 1);
            new->next = NULL;
            new->prev = NULL;

            if(!l->head){
                l->head = new;
                l->tail = new;
                return;    
            }

            new->prev = l->tail;
            l->tail->next = new;

            l->tail = new;
        case 1:
            Node *new = malloc(sizeof(*new));
            new->id = l->tail ? l->tail->id + 1 : 0;
            new->data = data;
            new->next = NULL;
            new->prev = NULL;

            if(!l->head){
                l->head = new;
                l->tail = new;
                return;    
            }

            new->prev = l->tail;
            l->tail->next = new;

            l->tail = new;
        default:
            return;
    }
}

int removeList(List *l, int id){
    switch(l->type){
        case 0:
        case 1:
        default:
            return 2;
    }
}

int getList(List *l, int id){
    switch(l->type){
        case 0:
        case 1:
        default:
            return 2;
    }
}

void freeList(List *l){
    Node *temp = NULL;
    for(Node *aux = l->head; aux != NULL; aux = aux->next){        
        if(temp) free(temp);
        temp = aux;
    }
    free(l);
    l = NULL;
}