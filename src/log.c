#include "log.h"

void logTrain(int train, int curr, int next) {
	char *path = buildPathTrainLogFile(train);
	char *log = getLogMessage(0, curr, next, NULL, 1);
	logOnFile(path, log);
}

void logRbc(int train, int curr, int next, char *response) {
	char *path = buildPathRbcLogFile();
	char *log = getLogMessage(train, curr, next, response, 0);
	logOnFile(path, log);
}

char *getLogMessage(int train, int curr, int next, char *response, int isTrain) {
	char *currName = decodeId(curr);
	char *nextName = decodeId(next);
	char *log;
	formatTime();
	if (isTrain) {
		log = csprintf(LOG_TRAIN_TEMPLATE, currName, nextName, timeBuffer);
	} else {
		log = csprintf(LOG_RBC_TEMPLATE, train, currName, nextName, response, timeBuffer);
	}
	free(currName);
	free(nextName);
	return log;
}

void formatTime(void) {
	time_t seconds = time(NULL);
	struct tm* tm_info = localtime(&seconds);
	strftime(timeBuffer, TIME_BUFFER_LENGTH, TIME_TEMPLATE, tm_info);
}

void logOnFile(char *path, char *log) {
	int fd = open(path, O_CREAT | O_WRONLY | O_APPEND, 0777);
	write(fd, log, strlen(log));
	close(fd);
	free(path);
	free(log);
}