#ifndef ROF
#define ROF

typedef struct router Router;

typedef struct link Link;

int **rlink(char id);
Router *rrouter(char num);
int countr();

#endif