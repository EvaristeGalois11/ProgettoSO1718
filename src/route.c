#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "route.h"
#include "common.h"

#define MA_PREFIX "MA"
#define MA_TEMPLATE MA_PREFIX"%d"
#define STATION_PREFIX "S"
#define STATION_TEMPLATE STATION_PREFIX"%d"
#define CSV_SEPARATOR ","

static Node *buildNode(char *);
static char *readLine(char *);

Node *generateRoute(char *path) {
	char *line = readLine(path);
	char *saveState;
	char *token = strtok_r(line, CSV_SEPARATOR, &saveState);
	Node *start = NULL;
	Node *current;
	while (token != NULL) {
		if (start == NULL) {
			start = buildNode(token);
			current = start;
		} else {
			current -> next = buildNode(token);
			current = current -> next;
		}
		token = strtok_r(NULL, CSV_SEPARATOR, &saveState);
	}
	free(line);
	return start;
}

char *readLine(char *path) {
	FILE * file = fopen(path, "r");
	int CURR_MAX = 40;
	char *buffer = malloc(sizeof(char) * CURR_MAX);
	int length = 0;
	char ch = getc(file);
	while ((ch != '\n') && (ch != EOF)) {
		if (length == CURR_MAX) {
			CURR_MAX *= 2;
			buffer = realloc(buffer, CURR_MAX);
		}
		if (ch != ' ') {
			buffer[length] = ch;
			length++;
		}
		ch = getc(file);
	}
	if (length == CURR_MAX) {
		buffer = realloc(buffer, ++CURR_MAX);
	}
	buffer[length] = '\0';
	return buffer;
}


Node *buildNode(char *name) {
	Node *node = malloc(sizeof(Node));
	node -> next = NULL;
	if (strstr(name , MA_PREFIX) != NULL) {
		sscanf(name, MA_TEMPLATE, &(node -> id));
	} else if (strstr(name , STATION_PREFIX) != NULL) {
		sscanf(name, STATION_TEMPLATE, &(node -> id));
		node -> id *= -1;
	} else {
		fprintf(stderr, "%s is not a valid string!\n", name);
	}
	return node;
}

char *decodeId(int id) {
	if (id < 0) {
		return csprintf(STATION_TEMPLATE, -id);
	} else if (id > 0) {
		return csprintf(MA_TEMPLATE, id);
	} else {
		return UNFREEABLE_STRING;
	}
}