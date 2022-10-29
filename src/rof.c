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

    // printf("%s %s %s\n", num_r, port, ip);
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
    char c;
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n')
            count++;
    if (c == EOF)
        count++;

    fclose(fp);
    return count;
}

int **rlink(char id) {// id do roteador atual
    char *filename = "../data/links.config";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Error: could not open file %s", filename);
        exit(1);
    }

    int graph = countr();
	int **matrix = malloc(sizeof(int *) * graph);
    for(int i = 0; i < graph; i++)
        matrix[i] = malloc(sizeof(i));
    for (int i = 0; i < graph; i++)
        for (int j = 0; j < graph; j++)
            matrix[i][j] = 0;
    // printf("%s %s %s\n", origin, destiny, size);
    char origin[2];
    char destiny[2];
    char size[3];
    while (fscanf(fp, "%s %s %s", origin, destiny, size) != EOF) {
        if (atoi(origin) == atoi(&id)) {
            matrix[atoi(origin) -1][atoi(destiny) -1] = atoi(size);
            matrix[atoi(destiny) -1][atoi(origin) -1] = atoi(size);
        }
    }

    fclose(fp);
    return matrix;
}

// int main() {
//     int graph = countr();
//     int **matrix = rlink('2');
//     for (int i = 0; i < graph; i++) {
//         for (int j = 0; j < graph; j++)
//             printf("%d ", matrix[i][j]);
//         printf("\n");
//     }

//     return 0;
// }