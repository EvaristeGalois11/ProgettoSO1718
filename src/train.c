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
static void writeOneByte(char *, char *);
static void move(int, int);

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
	// TODO lockare o sincronizzare? questo è il problema
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
	while (current != NULL) {
		// TODO aspettare gli altri treni
		Node *next = current -> next;
		int nextId = (next != NULL) ? next -> id : 0;
		logTrain(id, current -> id, nextId);
		if (requestModeEtcs1(id, current -> id, nextId)) {
			move(current -> id, nextId);
			current = next;
		}
		sleep(SLEEP_TIME);
	}
}

char readOneByte(char *path) {
	int descriptor = open(path, O_RDONLY);
	char byte[1];
	read(descriptor, byte, 1);
	close(descriptor);
	return byte[0];
}

void writeOneByte(char *path, char *byte) {
	int descriptor = open(path, O_RDWR);
	write(descriptor, byte, 1);
	close(descriptor);
}

void move(int curr, int next) {
	if (curr > 0) {
		char *currPath = buildPathMAxFile(curr);
		writeOneByte(currPath, "0");
		free(currPath);
	}
	if (next > 0) {
		char *nextPath = buildPathMAxFile(next);
		writeOneByte(nextPath, "1");
		free(nextPath);
	}
}