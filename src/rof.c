#include <stdio.h>
#include "rof.h"

Router *rrouter(int num) {
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
		if (num + 48 == (int)num_r[0])
            break;

    printf("%s %s %s\n", num_r, port, ip);
    router->id = num_r;
    router->port = port;
    router->ip = ip;

    return router;
}

// Link *rlink(int num) { // número para o rotedor escolhido
//     Link *link = malloc(sizeof(*link));
//     char *filename = "../data/links.config";
//     FILE *fp = fopen(filename, "r");

//     if (fp == NULL) {
//         printf("Error: could not open file %s", filename);
//         exit(1);
//     }

//     char origin[5];
//     char destination[6];
//     char size[15];
//     while (fscanf(fp, "%s %s %s", origin, destination, size) != EOF) {
// 		  if (num == (int)num_[0])
//         break;
//     }

//     printf("%s %s %s\n", num_, port, ip);
//     link->origin = origin;
//     link->destination = destination;
//     link->size = size;

//     return link;
// }

// int main() {
// 	rrouter(2);
// 	return 0;
// }