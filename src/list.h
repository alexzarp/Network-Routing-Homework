#ifndef LIST
#define LIST

#include <stdlib.h>

typedef struct list List;
typedef struct node Node;

// typedef struct cost Cost;

// contrutor: type recebe 1(lista normal) e 0(lista de listas)
List *buildList(int type);
// adiciona um valor qualquer e atrela um int id à ele
void addList(List *l, int id, int cost, int timeout, void *data);
// retorna o valor armazenado no nodo de id escolhido
void *getList(List *l, int id);
// remove nodo de identificador id e retorna 0 caso bem sucedido
int removeList(List *l, int id);
// libera espaço utilizado pela lista
void freeList(List *l);
// percorre a lista e executa uma função de leitura
void walksList(List *l, void (*walk)(int, void *));
// função de alta ordem: filter
List *filterList(List *l, int (*filter)(void *));
// função de alta ordem: map
List *mapList(List *l, void *(*mapper)(void *));
// função de alta ordem: reduce
void reduceList(List *l, void *acumulator,void (agregator)(void *, int, void *));

#endif /*LIST*/
