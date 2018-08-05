#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>

#define MAX_DIR_PATH "../resources/MAx"
#define MA_FILE_PREFIX "/MA"
#define ROUTES_DIR_PATH "../resources/routes"
#define ROUTE_FILE_PREFIX "/T"
#define LOG_DIR_PATH "../resources/log"
#define LOG_TRAIN_FILE_PREFIX "/T"
#define LOG_RBC_FILE_PREFIX "/RBC"
#define LOG_EXTENSION ".log"
#define TRAIN_SHARED_NAME "/traindata"
#define FILE_SERVER_PATH "../resources"
#define FILE_SERVER_NAME "/rbcSockets"
#define NUMBER_OF_TRAINS 5
#define NUMBER_OF_MA 16
#define ETCS1 "ETCS1"
#define	ETCS2 "ETCS2"
#define RBC	"RBC"

typedef struct shared_data_trains {
	pthread_mutex_t mutex;
	pthread_cond_t  condvar;
	int waiting;
	int completed;
} shared_data_trains;

char *exeDirPath;

char *truncExeName(char *);
char *buildPathMAxFile(int);
char *buildPathRouteFile(int);
char *buildPathTrainLogFile(int);
char *buildPathRbcLogFile(void);
char *csprintf(const char *, ...);
#endif