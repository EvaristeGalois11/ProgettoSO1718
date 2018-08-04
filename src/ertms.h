#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "common.h"

#define EXE_INFO_PATH "/proc/self/exe"
#define TRAIN_PROCESS_NAME "train"
#define NUMBER_OF_MA 16
#define RBC	"RBC"

char *getExePath();
void createDirIfNotExist(char *);
void createMAxFiles(int);
pid_t *startTrains(int, char *);
void waitTrainsTermination(int);
void launchETSC(char*);
void launchRBC();
void setUpSharedVariableForTrains();
void cleanUpSharedVariableForTrains();

extern char *exeDirPath;
shared_data_trains *data_trains;
