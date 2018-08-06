#ifndef TRAIN_H
#define TRAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "route.h"
#include "common.h"
#include "log.h"

#define SLEEP_TIME 3

typedef int (*request_mode)(int, int, int);

int main(int, char **);
void setUpSharedVariable(void);
void cleanUp(void);
Node *readAndDecodeRoute(void);
int requestModeEtcs1(int, int, int);
int checkMAxFile(int id);
int requestModeEtcs2(int, int, int);
void startTravel(void);
void waitOtherTrains(void);
void checkOtherTrains(int *, int);
void eLUltimoChiudaLaPorta(void);
void travelCompleted(void);
void lockExclusiveMA(int, int *);
void unlockFile(int *);
void move(void);
void writeOneByte(int, char *);

int trainId;
Node *start;
Node *current;
request_mode requestMode;
extern char *exeDirPath;
shared_data_trains *data_trains;
int currDescriptor = -1;
int nextDescriptor = -1;
struct flock writeLock = {
	.l_type = F_WRLCK,
	.l_whence = SEEK_SET,
	.l_start = 0,
	.l_len = 1,
};
#endif