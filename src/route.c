#include "route.h"

Node *generateRoute(char *path) {
	char *line = readLine(path);
	char *saveState;
	char *token = strtok_r(line, CSV_SEPARATOR, &saveState);
	Node *start = buildNode(token);
	Node *current = start;
	while (current != NULL) {
		token = strtok_r(NULL, CSV_SEPARATOR, &saveState);
		current -> next = buildNode(token);
		current = current -> next;
	}
	free(line);
	return start;
}

char *readLine(char *path) {
	FILE * file = fopen(path, "r");
	int MAX = 40;
	char *buffer = malloc(sizeof(char) * MAX);
	int length = 0;
	char ch = getc(file);
	while ((ch != '\n') && (ch != EOF)) {
		if (length == MAX) {
			MAX *= 2;
			buffer = realloc(buffer, MAX);
		}
		if (ch != ' ') {
			buffer[length] = ch;
			length++;
		}
		ch = getc(file);
	}
	if (length == MAX) {
		buffer = realloc(buffer, MAX + 1);
	}
	buffer[length] = '\0';
	fclose(file);
	return buffer;
}


Node *buildNode(char *name) {
	Node *node = malloc(sizeof(Node));
	node -> next = NULL;
	if (name == NULL) {
		free(node);
		node = NULL;
	} else if (strstr(name , MA_PREFIX) != NULL) {
		sscanf(name, MA_TEMPLATE, &node -> id);
	} else if (strstr(name , STATION_PREFIX) != NULL) {
		sscanf(name, STATION_TEMPLATE, &node -> id);
		node -> id *= -1;
	}
	return node;
}

char *decodeId(int id) {
	if (id < 0) {
		return csprintf(STATION_TEMPLATE, -id);
	} else if (id > 0) {
		return csprintf(MA_TEMPLATE, id);
	} else {
		return csprintf(NOT_APPLICABLE);
	}
}

void destroyRoute(Node *node) {
	Node *temp;
	do {
		temp = node -> next;
		free(node);
		node = temp;
	} while (node != NULL);
}