#ifndef STATUS
#define STATUS

#include "control.h"

typedef struct status Status;

Status *buildStatus(int rid, int size);
int getStatus(Status *s, int rid);
void setStatus(Status *s, int rid, int value);
void freeStatus(Status *s);

#endif /*STATUS*/