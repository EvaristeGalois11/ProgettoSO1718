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
#include "log.h"

int main(int, char **);
void setUpSharedVariable(void);
void cleanUp(void);
int createSocket(const char*);
void startServeTrain(int);
int waitForPosition(int);
void waitForRequest(int);
Node *importRoutes(void);

shared_data_rbc *data_rbc;
int ma[NUMBER_OF_MA] = {0};
shared_data_rbc *dataRbc;
int currLock = -1;
int nextLock = -1;
#endif