#ifndef STATUS
#define STATUS

#include "control.h"

typedef struct status Status;

Status *buildStatus(int rid);
int getStatus(Status *s, int rid);
void setStatus(Status *s, int rid, int value);
void freeStatus(Status *s);
void displayStatus(Status *s, int rid);

#endif /*STATUS*/