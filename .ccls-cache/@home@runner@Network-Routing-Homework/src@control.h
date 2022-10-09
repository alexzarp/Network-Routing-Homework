#ifndef CONTROL
#define CONTROL

typedef struct {
    int type;
    char *root;
    char *destiny;
    void *payload;
}Message;

typedef struct node{
    Message *msg;
    struct node *next;
}Node;

typedef struct{
    Node *head;
    Node *tail;
}Queue;

Queue *buildQueue(int type);
#endif /*CONTROL*/