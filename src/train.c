#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "route.h"
#include "common.h"
#include "log.h"

#define SLEEP_TIME 3

typedef int (*request_mode)(int, int, int);
typedef struct flock flock;

static int id;
static Node *current;
static request_mode requestMode;
extern char *exeDirPath;

static void setUpExeDirPath(char *exePath);
static Node *readAndDecodeRoute();
static int requestModeEtcs1(int, int, int);
static int requestModeEtcs2(int, int, int);
static int checkMAxFile(int id);
static void startTravel();
static char readOneByte(char *);
static void writeOneByte(int, char *);
int lockExclusiveFile(char *);
static int move(int, int);

flock writeLock = {
	.l_type = F_WRLCK,
	.l_whence = SEEK_SET,
	.l_start = 0,
	.l_len = 1,
};

int main(int argc, char *argv[]) {
	setUpExeDirPath(argv[0]);
	id = toInt(argv[1]);
	requestMode = (strcmp(argv[2], "ETCS1") == 0) ? requestModeEtcs1 : requestModeEtcs2;
	current = readAndDecodeRoute();
	startTravel();
	return 0;
}

void setUpExeDirPath(char *exePath) {
	char *temp = malloc(sizeof(char) * strlen(exePath) + 1);
	strcpy(temp, exePath);
	exeDirPath = truncExeName(temp);
}

Node *readAndDecodeRoute() {
	char *path = buildPathRouteFile(id);
	Node *result = generateRoute(path);
	free(path);
	return result;
}

int requestModeEtcs1(int train, int curr, int next) {
	if (next <= 0) {
		return 1;
	} else {
		return checkMAxFile(next);
	}
}

int checkMAxFile(int id) {
	char *path = buildPathMAxFile(id);
	char byte = readOneByte(path);
	free(path);
	return (byte == '0');
}

int requestModeEtcs2(int train, int curr, int next) {
	// TODO implementare richiesta al server RBC
	return 0;
}

void startTravel() {
	do {
		// TODO aspettare gli altri treni
		printf("nuovo giro %d\n", id);
		logTrain(id, current -> id, current -> next -> id);
		if (requestModeEtcs1(id, current -> id, current -> next -> id)) {
			if (move(current -> id, current -> next -> id)) {
				current = current -> next;
			}
		}
		sleep(SLEEP_TIME);
	} while (current -> id > 0);
	logTrain(id, current -> id, 0);
}

char readOneByte(char *path) {
	int descriptor;
	if ((descriptor = lockExclusiveFile(path)) == -1) {
		return '1';
	}
	printf("Treno: %d - Inizio lettura file: %s\n", id, path);
	char byte[1];
	read(descriptor, byte, 1);
	printf("Treno: %d - Fine lettura file: %s\n", id, path);
	close(descriptor);
	return byte[0];
}

void writeOneByte(int descriptor, char *byte) {
	write(descriptor, byte, 1);
	fsync(descriptor);
	close(descriptor);
}

int lockExclusiveFile(char *path) {
	int descriptor = open(path, O_RDWR);
	if ((fcntl (descriptor, F_SETLK, &writeLock) == -1)) {
		printf("Treno: %d - Impossibile lock file: %s\n", id, path);
		return -1;
	}
	return descriptor;
}

int move(int curr, int next) {
	char *currPath = buildPathMAxFile(curr);
	char *nextPath = buildPathMAxFile(next);

	int descriptor1 = (currPath == NULL) ? 0 : lockExclusiveFile(currPath);
	int descriptor2 = (nextPath == NULL) ? 0 : lockExclusiveFile(nextPath);

	if (descriptor1 < 0 || descriptor2 < 0) {
		if (currPath != NULL && descriptor1 > 0) {
			close(descriptor1);
		}
		if (nextPath != NULL && descriptor2 > 0) {
			close(descriptor2);
		}
		return 0;
	}

	if (currPath != NULL) {
		printf("Treno: %d - Inizio scrittura file: %s\n", id, currPath);
		writeOneByte(descriptor1, "0");
		printf("Treno: %d - Fine scrittura file: %s\n", id, currPath);
		free(currPath);
	}
	if (nextPath != NULL) {
		printf("Treno: %d - Inizio scrittura file: %s\n", id, nextPath);
		writeOneByte(descriptor2, "1");
		printf("Treno: %d - Fine scrittura file: %s\n", id, nextPath);
		free(nextPath);
	}
	return 1;
}

