#include <stdio.h>

int rrouter(char num) { // número para o rotedor escolhido
    char *filename = "../data/routers.config";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Error: could not open file %s", filename);
        return 1;
    }

    char a[100]
	char 
	 // fscanf(arq, "%d%f", &i, &x)
    while (fscanf(fp, "%s %s %s", &buf[0][0], &buf[0][1], &buf[0][2]) != EOF)
		printf("%s\n", buf);

    return 0;
}

// char rlink() {
// 	return var;
// }

int main() {
	rrouter('2');
	return 0;
}