#ifndef ERTMS_H
#define ERTMS_H
#include <stdio.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "common.h"

#define EXE_INFO_PATH "/proc/self/exe"
#define TRAIN_PROCESS_NAME "train"
#define RBC_PROCESS_NAME "rbc"

int main(int, char **);
char *getExePath(void);
void launchEtsc(char*);
void setUpSharedVariableForTrains(void);
void cleanUpSharedVariableForTrains(void);
void launchRbc(void);
void setUpSharedVariableForRbc(void);
void cleanUpSharedVariableForRbc(void);
void createMAxFiles(void);
void startTrains(char *);
void startRbc(void);
void sendRoutes(void);

extern char *exeDirPath;
shared_data_trains *dataTrains;
shared_data_rbc *dataRbc;
#endif