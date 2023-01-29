#include "rof.h"
#include "control.h"

struct router{
    char *id;
    char *port;
    char *ip;
};

struct link{
    char *origin;
    char *destination;
    char *size;
};

Router *rrouter(char num) {
    Router *router = malloc(sizeof(*router));
    char *filename = "data/routers.config";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        die("fopen");
    }

    char num_r[2];
    char port[6];
    char ip[15];
    while (fscanf(fp, "%s %s %s", num_r, port, ip) != EOF)
		if (atoi(&num) == atoi(num_r))
            break;

    router->id = num_r;
    router->port = port;
    router->ip = ip;

    fclose(fp);
    return router;
}

int countr() {
    char *filename = "data/routers.config";
    FILE *fp = fopen(filename, "r");

    int count = 0;
    char c;
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n')
            count++;
    if (c == EOF)
        count++;

    fclose(fp);
    return count;
}

// id do roteador atual
List *rlink(char id) {
    char *filename = "data/links.config";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        die("fopen");
    }

// 1 - Mensagem | 0 - Controle
	List *matrix = buildList(0);
    List *distance_vector = buildList(1);
    int *data = malloc(sizeof(int));
    *data = 0;

    addList(matrix, atoi(&id), 0, (void *)distance_vector);
    addList(distance_vector, atoi(&id), 0,  (void *)data);

    char origin[2];
    char destiny[2];
    char size[3];
    while (fscanf(fp, "%s %s %s", origin, destiny, size) != EOF) {
        if (atoi(origin) == atoi(&id) || atoi(destiny) == atoi(&id)) {
            data = malloc(sizeof(int));
            *data = atoi(size);
            addList(distance_vector, atoi(origin) == atoi(&id) ? atoi(destiny) : atoi(origin), atoi(size), (void *)data);
        }
    }

    fclose(fp);
    return matrix;
}