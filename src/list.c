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

static Node *getNode(List *l, int id){
    for(Node *aux = l->head; aux != NULL; aux = aux->next){
			if(aux->id == id) return aux;
		}
		return NULL;
}

void addList(List *l, void *data){
		Node *new;
    switch(l->type){
        case 0:
            new = malloc(sizeof(*new));
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
        case 1:
            new = malloc(sizeof(*new));
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
		if(!l->head) return 1;
		Node *n;
		switch(l->type){
			case 0:
				if(l->head->id == id){
        	n = l->head;
        	if(!l->head->next){
							freeList((List *)n->data);
            	free(n);
            	l->head = NULL;
            	l->tail = NULL;
            	return 0;
        	}
        	l->head = l->head->next;
        	l->head->prev = NULL;
					n->prev->next = n->next;
					n->next->prev = n->prev;
					freeList((List *)n->data);
        	free(n);
        	return 0;
    		}

    		if(l->tail->id == id){
        		n = l->tail;
        		l->tail = n->prev;
        		l->tail->next = NULL;
						freeList((List *)n->data);
        		free(n);
        		return 0;
    		}
				n = getNode(l,id);
				if(!n) return 1;
				freeList((List *)n->data);
				free(n);
				return 0;
			case 1:
				if(l->head->id == id){
        	n = l->head;
        	if(!l->head->next){
            	free(n);
            	l->head = NULL;
            	l->tail = NULL;
            	return 0;
        	}
        	l->head = l->head->next;
        	l->head->prev = NULL;
        	free(n);
        	return 0;
    		}

    		if(l->tail->id == id){
        		n = l->tail;
        		l->tail = n->prev;
        		l->tail->next = NULL;
        		free(n);
        		return 0;
    		}
				n = getNode(l,id);
				if(!n) return 1;
				n->prev->next = n->next;
				n->next->prev = n->prev;
				free(n);
				return 0;

			default:
				return 2;
    }
}

void *getList(List *l, int id){
    Node *n = getNode(l,id);
		return n ? n->data : NULL;
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
