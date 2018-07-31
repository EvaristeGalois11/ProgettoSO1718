#define MAX_DIR_PATH "../resources/MAx"
#define MA_FILE_PREFIX "/MA"
#define ROUTES_DIR_PATH "../resources/routes"
#define ROUTE_FILE_PREFIX "/T"

int countDigits(int);
int toInt(char *);
char *truncExeName(char *);
int getPathMAxFileLength(int);
void formatPathMAxFile(char **, int);
int getPathRouteFileLength(int);
void formatPathRouteFile(char **, int);