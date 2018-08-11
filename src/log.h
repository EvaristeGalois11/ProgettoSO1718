#ifndef LOG_H
#define LOG_H
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "common.h"
#include "route.h"

#define LOG_TRAIN_TEMPLATE "[Attuale: %s], [Prossimo: %s], %s\n"
#define LOG_RBC_TEMPLATE "[Treno richiedente autorizzazione: T%d], [Segmento attuale: %s], [Segmento richiesto: %s], [Autorizzato: %s], [Data: %s]\n"
#define TIME_TEMPLATE "%d %B %Y %H:%M:%S"
#define TIME_BUFFER_LENGTH 30

void logTrain(int, int, int);
void logRbc(int, int, int, char *);
char *getLogMessage(int, int, int, char *, int);
void formatTime(void);
void logOnFile(char *, char *);

char timeBuffer[TIME_BUFFER_LENGTH];
#endif