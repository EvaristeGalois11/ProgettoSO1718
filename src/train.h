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

void setUpSharedVariable();
void setUpExeDirPath(char *exePath);
Node *readAndDecodeRoute();
int requestModeEtcs1(int, int, int);
int requestModeEtcs2(int, int, int);
int checkMAxFile(int id);
void startTravel();
void waitOtherTrains();
void eLUltimoChiudaLaPorta();
void travelCompleted();
void writeOneByte(int, char *);
void lockExclusiveMA(int, int *);
void unlockFile(int *);
void move();
void cleanUpSharedVariable();

int trainId;
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