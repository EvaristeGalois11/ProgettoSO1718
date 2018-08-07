#ifndef ROUTE_H
#define ROUTE_H
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "common.h"

#define MA_PREFIX "MA"
#define MA_TEMPLATE MA_PREFIX"%d"
#define STATION_PREFIX "S"
#define STATION_TEMPLATE STATION_PREFIX"%d"
#define CSV_SEPARATOR ","
#define NOT_APPLICABLE "---"

typedef struct Node {
	int id;
	struct Node *next;
} Node;

Node *generateRoute(char *);
char *readLine(int);
Node *buildNode(char *);
char *decodeId(int);
void destroyRoute(Node *);
#endif