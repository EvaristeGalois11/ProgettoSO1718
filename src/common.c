#include "common.h"

char *truncExeName(char *exePath) {
	char *lastOccurance = NULL;
	char *temp = exePath;
	do {
		lastOccurance = temp;
		temp = strstr(++lastOccurance, "/");
	} while (temp != NULL);
	lastOccurance[0] = '\0';
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

char *buildPathRbcLogFile(void) {
	return csprintf("%s%s%s%s", exeDirPath, LOG_DIR_PATH, LOG_RBC_FILE_PREFIX, LOG_EXTENSION);
}

char *csprintf(const char *format, ...) {
	va_list arglist1, arglist2;
	va_start(arglist1, format);
	va_copy(arglist2, arglist1);
	int needed = vsnprintf(NULL, 0, format, arglist1);
	char *string = malloc(needed + 1);
	vsprintf(string, format, arglist2);
	va_end(arglist1);
	va_end(arglist2);
	return string;
}