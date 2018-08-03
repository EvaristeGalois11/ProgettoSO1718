#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include "log.h"
#include "common.h"
#include "route.h"

#define LOG_TRAIN_TEMPLATE "[Attuale: %s], [Next: %s], %s\n"
#define LOG_RBC_TEMPLATE "[TRENO RICHIEDENTE AUTORIZZAZIONE: T%d], [SEGMENTO ATTUALE: %s], [SEGMENTO RICHIESTO: %s], [AUTORIZZATO: %s], [DATA: %s]\n"
#define TIME_TEMPLATE "%d %B %Y %H:%M:%S"
#define TIME_BUFFER_LENGTH 30

static char *getLogMessage(int, int, int, char *, int);
static void formatTime();
static void logOnFile(char *, char *);

static char timeBuffer[TIME_BUFFER_LENGTH];

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
	if (strcmp(nextName, UNFREEABLE_STRING) != 0) {
		free(nextName);
	}
	return log;
}

void formatTime() {
	time_t seconds = time(NULL);
	struct tm* tm_info = localtime(&seconds);
	setlocale(LC_TIME, "");
	strftime(timeBuffer, TIME_BUFFER_LENGTH, TIME_TEMPLATE, tm_info);
}

void logOnFile(char *path, char *log) {
	int fd = open(path, O_CREAT | O_WRONLY | O_APPEND, 0777);
	write(fd, log, strlen(log));
	close(fd);
	free(path);
	free(log);
}