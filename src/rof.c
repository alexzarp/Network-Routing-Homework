#include <stdio.h>
#include <stdlib.h>
#include "rof.h"
// #include "control.h"

Router *rrouter(char num) {
    Router *router = malloc(sizeof(*router));
    char *filename = "../data/routers.config";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Error: could not open file %s", filename);
        exit(1);
    }

    char num_r[2];
    char port[6];
    char ip[15];
    while (fscanf(fp, "%s %s %s", num_r, port, ip) != EOF)
		if (num == num_r[0])
            break;

    printf("%s %s %s\n", num_r, port, ip);
    router->id = num_r;
    router->port = port;
    router->ip = ip;

    fclose(fp);
    return router;
}

int countr() {
    char *filename = "../data/routers.config";
    FILE *fp = fopen(filename, "r");

    int count = 0;
    for (char c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n')
            count++;

    fclose(fp);
    return count;
}

int **rlink() {
    char *filename = "../data/links.config";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Error: could not open file %s", filename);
        exit(1);
    }

    char origin[2];
    char destiny[2];
    char size[3];

    int graph = countr();
	int **matrix = malloc(sizeof(int *) * graph);
    for(int i = 0; i < graph; i++)
        matrix[i] = malloc(sizeof(i) * graph);

    for (int i = 0; i < graph; i++)
        for (int j = 0; i < graph; j++)
            matrix[i][j] = 0;

    printf("%s %s %s\n", origin, destiny, size);

    fclose(fp);
    return matrix;
}

int main() {
    int graph = countr();
    int **matrix = rlink();
    return 0;
}