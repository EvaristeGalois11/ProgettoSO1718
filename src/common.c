#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "common.h"

char *exeDirPath;

static char *buildGenericPath(const char *, ...);

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

char *buildPathMAxFile(int id) {
	return buildGenericPath("%s%s%s%d", exeDirPath, MAX_DIR_PATH, MA_FILE_PREFIX, id);
}

char *buildPathRouteFile(int id) {
	return buildGenericPath("%s%s%s%d", exeDirPath, ROUTES_DIR_PATH, ROUTE_FILE_PREFIX, id);
}

char *buildGenericPath(const char *format, ...) {
	va_list arglist1, arglist2;
	va_start(arglist1, format);
	va_copy(arglist2, arglist1);
	int needed = vsnprintf(NULL, 0, format, arglist1);
	char  *path = malloc(needed + 1);
	vsprintf(path, format, arglist2);
	va_end(arglist1);
	va_end(arglist2);
	return path;
}