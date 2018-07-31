#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "route.h"
#include "common.h"

static int id;
static char *mode;
static Node *current;
extern char *exeDirPath;

static void setUpExeDirPath(char *exePath);
static Node *readAndDecodeRoute();

int main(int argc, char *argv[]) {
	setUpExeDirPath(argv[0]);
	id = toInt(argv[1]) + 1;
	mode = argv[2];

	printf("treno numero %d in directory %s avviato in modalità %s\n", id, exeDirPath, mode);

	current = readAndDecodeRoute();

	while (current != NULL) {
		printf("%d\n", current -> id);
		current = current -> next;
	}

	return 0;
}

void setUpExeDirPath(char *exePath) {
	char *temp = malloc(sizeof(char) * strlen(exePath) + 1);
	strcpy(temp, exePath);
	exeDirPath = truncExeName(temp);
}

Node *readAndDecodeRoute() {
	int length = getPathRouteFileLength(id);
	char *path = malloc(sizeof(char) * length);
	formatPathRouteFile(&path, id);
	Node *result = generateRoute(path);
	free(path);
	return result;
}