#ifndef RBC_H
#define RBC_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include "route.h"
#include "common.h"

int main(int, char **);
void setUpSharedVariable(void);
void cleanUp(void);
int createSocket(const char*);

shared_data_rbc *data_rbc;
#endif