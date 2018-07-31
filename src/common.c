#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "common.h"

char *exeDirPath;

static int getGenericPathLength(int, ...);

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
	return getGenericPathLength(id, exeDirPath, MAX_DIR_PATH, MA_FILE_PREFIX, NULL);
}

int getPathRouteFileLength(int id) {
	return getGenericPathLength(id, exeDirPath, ROUTES_DIR_PATH, ROUTE_FILE_PREFIX, NULL);
}

int getGenericPathLength(int num, ...) {
	va_list argp;
	va_start(argp, num);
	int result = 0;
	char *string = va_arg(argp, char *);
	while (string != NULL) {
		result += strlen(string);
		string = va_arg(argp, char *);
	}
	va_end(argp);
	return result;
}

void formatPathMAxFile(char **path, int id) {
	sprintf(*path, "%s%s%s%d", exeDirPath, MAX_DIR_PATH, MA_FILE_PREFIX, id);
}

void formatPathRouteFile(char **path, int id) {
	sprintf(*path, "%s%s%s%d", exeDirPath, ROUTES_DIR_PATH, ROUTE_FILE_PREFIX, id);
}
