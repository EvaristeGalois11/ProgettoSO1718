#include "route.h"

Node *generateRoute(char *line) {
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

char *readLine(int fd) {
	int length = 50;
	char *str = malloc(sizeof(char) * length);
	int index = 0;
	int n = 0;
	char temp;
	do {
		if (index == length) {
			length *= 2;
			str = realloc(str, length);
		}
		n = read(fd, &temp, 1);
		if (n == 0) {
			return NULL;
		}
		if (temp != ' ') {
			str[index++] = temp;
		}
	} while (temp != '\n' && temp != EOF);
	str[--index] = '\0';
	printf("%s\n", str);
	return str;
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