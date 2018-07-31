#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "common.h"

#define EXE_INFO_PATH "/proc/self/exe"
#define TRAIN_PROCESS_NAME "train"
#define NUMBER_OF_MA 16
#define NUMBER_OF_TRAINS 1
#define ETCS1 "ETCS1"
#define	ETCS2 "ETCS2"
#define RBC	"RBC"

typedef enum {other, etcs, rbc} launchmode;

static int checkLaunchMode(int, char*[]);
static char *getExePath();
static void createDirIfNotExist(char *);
static void createMAxFiles(int);
static pid_t *startTrains(int, char *);
static void waitTrainsTermination();

extern char *exeDirPath;

int main(int argc, char *argv[]) {
	launchmode launchMode = checkLaunchMode(argc, argv);
	if (launchMode == other) {
		printf("selezionare un modo per lanciare il programma ETCS1 | ETCS2 | ETCS2 RBC\n");
		return 0;
	} else if (launchMode == rbc) {
		printf("stiamo avviando il server Kappa :)\n");
		return 0;
	}
	exeDirPath = truncExeName(getExePath());
	mode_t previousMask = umask(0000);
	createDirIfNotExist(MAX_DIR_PATH);
	createMAxFiles(NUMBER_OF_MA);
	pid_t *pids = startTrains(NUMBER_OF_TRAINS, argv[1]);
	waitTrainsTermination();
	free(pids);
	umask(previousMask);
	return 0;
}

int checkLaunchMode(int argc, char *argv[]) {
	if (argc == 2 && (strcmp(argv[1], ETCS1) == 0 || strcmp(argv[1], ETCS2) == 0))
		return 1;
	else if (argc == 3 && strcmp(argv[1], ETCS2) == 0 && strcmp(argv[2], RBC) == 0)
		return 2;
	return 0;
}

char *getExePath() {
	int size = 10;
	char *buffer = NULL;
	while (1) {
		buffer = realloc(buffer, size);
		int nchars = readlink (EXE_INFO_PATH, buffer, size);
		if (nchars < 0) { //errore se nchars=-1
			free(buffer);
			return NULL;
		} else if (nchars < size) {
			return buffer;
		} else {
			size *= 2;
		} //se nchars==size il filename e' stato troncato e ha bisogno di più spazio
	}
}

void createDirIfNotExist(char *dir) {
	struct stat st;
	int len = strlen(exeDirPath) + strlen(dir) + 1;
	char pathExe[len];
	strcpy(pathExe, exeDirPath);
	strcat(pathExe, dir);
	if (stat(pathExe, &st) == -1) {
		mkdir(pathExe, 0777);
	}
}

void createMAxFiles(int num) {
	char *path = malloc(sizeof(char) * getPathMAxFileLength(num));
	for (int i = 1; i < num + 1; i++) {
		formatPathMAxFile(&path, i);
		int fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0777);
		write(fd, "0", 1);
		close(fd);
	}
	free(path);
}

pid_t *startTrains(int num, char *mode) {
	num++;
	int maxDigits = countDigits(num);
	pid_t *pids = malloc(sizeof(pid_t) * num);
	char id[maxDigits + 1];
	for (int i = 0; i < num; i++) {
		if ((pids[i] = fork()) == 0) {
			int len = strlen(exeDirPath) + strlen(TRAIN_PROCESS_NAME) + 1;
			char pathExe[len];
			sprintf(pathExe, "%s%s", exeDirPath, TRAIN_PROCESS_NAME);
			sprintf(id, "%d", i);
			execl(pathExe, pathExe, id, mode, NULL);
			perror("Child");
			exit(errno);
		}
	}
	return pids;
}

void waitTrainsTermination() {
	int status;
	pid_t pid;
	for (int i = NUMBER_OF_TRAINS; i > 0; i--) {
		pid = wait(&status);
		printf("Child with PID %ld exited with status 0x%x\n", (long) pid, status);
	}
}