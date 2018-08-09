#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define MAX_DIR_PATH "../resources/MAx"
#define MA_FILE_PREFIX "/MA"
#define ROUTES_DIR_PATH "../resources/routes"
#define ROUTE_FILE_PREFIX "/T"
#define LOG_DIR_PATH "../resources/log"
#define LOG_TRAIN_FILE_PREFIX "/T"
#define LOG_RBC_FILE_PREFIX "/RBC"
#define LOG_EXTENSION ".log"
#define SOCKET_DIR_PATH "../resources/server"
#define TRAIN_SOCKET_NAME "/trainsocket"
#define ERTMS_SOCKET_NAME "/ertmssocket"
#define TRAIN_SHARED_NAME "/traindata"
#define RBC_SHARED_NAME "/rbcdata"
#define NUMBER_OF_TRAINS 5
#define NUMBER_OF_MA 16
#define NUMBER_OF_STATIONS 8
#define OK "SI"
#define KO "NO"
#define ETCS1 "ETCS1"
#define	ETCS2 "ETCS2"
#define RBC	"RBC"

typedef struct shared_data_trains {
	pthread_mutex_t mutex;
	pthread_cond_t  condvar;
	int waiting;
	int completed;
} shared_data_trains;

typedef struct shared_data_rbc {
	pthread_mutex_t mutexes[NUMBER_OF_MA];
	int ma[NUMBER_OF_MA];
	int stations[NUMBER_OF_STATIONS];
} shared_data_rbc;

char *buildPathMAxFile(int);
char *buildPathRouteFile(int);
char *buildPathTrainLogFile(int);
char *buildPathRbcLogFile(void);
char *buildPathTrainSocketFile(void);
char *buildPathErtmsSocketFile(void);
char *csprintf(const char *, ...);
void setUpExeDirPath(char *exePath);
char *truncExeName(char *);
void createDirIfNotExist(char *);
void waitChildrenTermination(int);
int setUpSocket(char *, int);

char *exeDirPath;
#endif