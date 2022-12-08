#ifndef ROF
#define ROF

typedef struct router Router; // Roteadores

typedef struct link Link; // Conexões entre roteadores

int **rlink(char id); // lê dados do roteador id e retorna uma matrix com suas conexões
Router *rrouter(char num); // lê dados do roteador id e retorna uma struct Router referente a ele
int countr(); // conta o número de roteadores na rede

#endif /*ROF*/