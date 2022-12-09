#ifndef LIST
#define LIST

#include "control.h"

typedef struct list List;

List *buildList(int type);
void addList(List *l, void *data);
void *getList(List *l, int id);
int removeList(List *l, int id);
void freeList(List *l);

#endif /*LIST*/
