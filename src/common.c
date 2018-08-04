#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "common.h"

int countDigits(int n) { // inutile più avanti
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
	if (id > 0) {
		return csprintf("%s%s%s%d", exeDirPath, MAX_DIR_PATH, MA_FILE_PREFIX, id);
	} else {
		return NULL;
	}
}

char *buildPathRouteFile(int id) {
	return csprintf("%s%s%s%d", exeDirPath, ROUTES_DIR_PATH, ROUTE_FILE_PREFIX, id);
}

char *buildPathTrainLogFile(int id) {
	return csprintf("%s%s%s%d%s", exeDirPath, LOG_DIR_PATH, LOG_TRAIN_FILE_PREFIX, id, LOG_EXTENSION);
}

char *buildPathRbcLogFile() {
	return csprintf("%s%s%s%s", exeDirPath, LOG_DIR_PATH, LOG_RBC_FILE_PREFIX, LOG_EXTENSION);
}

char *csprintf(const char *format, ...) {
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