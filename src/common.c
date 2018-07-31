#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "common.h"

char *exeDirPath;

int countDigits(int n) {
	if (n == 0) {
		return 1;
	} else {
		return (int) log10(n) + 1;
	}
}

int toInt(char *string) {
	char *next;
	return (int) strtol(string, &next, 10);
}

char *truncExeName(char *exePath) {
	int len = strlen(exePath);
	while (exePath[len] != '/' && len >= 0) {
		len--;
	}
	if (len == 0) {
		return exePath;
	}
	exePath = realloc(exePath, ++len);
	exePath[len] = '\0';
	return exePath;
}

int getPathMAxFileLength(int id) {
	int digits = countDigits(id);
	return strlen(exeDirPath) + strlen(MAX_DIR_PATH) + strlen(MA_FILE_PREFIX) + digits + 1;
}

void formatPathMAxFile(char **path, int id) {
	sprintf(*path, "%s%s%s%d", exeDirPath, MAX_DIR_PATH, MA_FILE_PREFIX, id);
}

int getPathRouteFileLength(int id) {
	int digits = countDigits(id);
	return strlen(exeDirPath) + strlen(ROUTES_DIR_PATH) + strlen(ROUTE_FILE_PREFIX) + digits + 1;
}

void formatPathRouteFile(char **path, int id) {
	sprintf(*path, "%s%s%s%d", exeDirPath, ROUTES_DIR_PATH, ROUTE_FILE_PREFIX, id);
}
