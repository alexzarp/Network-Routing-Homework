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
int **rlink(char id) {
    char *filename = "data/links.config";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        die("fopen");
    }
    int graph = countr();

	int **matrix = malloc(sizeof(int *) * graph);
    for(int i = 0; i < graph; i++) matrix[i] = malloc(sizeof(int) * graph);

    for (int i = 0; i < graph; i++)
        for (int j = 0; j < graph; j++)
            matrix[i][j] = 0;
    char origin[2];
    char destiny[2];
    char size[3];
    while (fscanf(fp, "%s %s %s", origin, destiny, size) != EOF) {
        if (atoi(origin) == atoi(&id) || atoi(destiny) == atoi(&id)) {
            matrix[atoi(origin) -1][atoi(destiny) -1] = atoi(size);
            matrix[atoi(destiny) -1][atoi(origin) -1] = atoi(size);
        }
    }

    fclose(fp);
    return matrix;
}