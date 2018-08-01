#define MAX_DIR_PATH "../resources/MAx"
#define MA_FILE_PREFIX "/MA"
#define ROUTES_DIR_PATH "../resources/routes"
#define ROUTE_FILE_PREFIX "/T"
#define LOG_DIR_PATH "../resources/log"
#define LOG_TRAIN_FILE_PREFIX "/T"
#define LOG_RBC_FILE_PREFIX "/RBC"
#define LOG_EXTENSION ".log"

int countDigits(int);
int toInt(char *);
char *truncExeName(char *);
char *buildPathMAxFile(int);
char *buildPathRouteFile(int);
char *buildPathTrainLogFile(int);
char *buildPathRbcLogFile();
char *csprintf(const char *, ...);