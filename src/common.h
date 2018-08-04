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
#define NUMBER_OF_TRAINS 5
#define ETCS1 "ETCS1"
#define	ETCS2 "ETCS2"

typedef struct shared_data_trains {
	pthread_mutex_t mutex;
	pthread_cond_t  condvar;
	int waiting;
	int completed;
} shared_data_trains;

char *exeDirPath;

int countDigits(int);
int toInt(char *);
char *truncExeName(char *);
char *buildPathMAxFile(int);
char *buildPathRouteFile(int);
char *buildPathTrainLogFile(int);
char *buildPathRbcLogFile();
char *csprintf(const char *, ...);