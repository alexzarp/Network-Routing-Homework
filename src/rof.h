#ifndef ROF
#define ROF

typedef struct {
    char *id;
    char *port;
    char *ip;
}Router;

typedef struct {
    char *origin;
    char *destination;
    char *size;
}Link;

int **rlink(char id);
Router *rrouter(char num);
int countr();

#endif