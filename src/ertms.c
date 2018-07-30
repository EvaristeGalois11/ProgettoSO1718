#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

#define MAX_DIR_PATH "../resources/MAx"
#define MA_FILE_PREFIX "/MA"
#define TRAIN_PROCESS_NAME "train"
#define TRAIN_PROCESS_PATH "./"TRAIN_PROCESS_NAME
#define NUMBER_OF_MA 16
#define NUMBER_OF_TRAINS 5

static void createDirIfNotExist(char *);
static void createMAxFiles(int);
static int countDigits(int);
static pid_t *startTrains(int, char *);
static void waitTrainsTermination();

int main(int argc, char *argv[]) {
	mode_t previousMask = umask(0000);
	createDirIfNotExist(MAX_DIR_PATH);
	createMAxFiles(NUMBER_OF_MA);
	pid_t *pids = startTrains(NUMBER_OF_TRAINS, argv[1]);
	waitTrainsTermination();
	free(pids);
	umask(previousMask);
	return 0;
}

void createDirIfNotExist(char *dir) {
	struct stat st;
	if (stat(dir, &st) == -1) {
		mkdir(dir, 0777);
	}
}

void createMAxFiles(int num) {
	int maxDigits = countDigits(num);
	int maxPathLength = strlen(MAX_DIR_PATH) + strlen(MA_FILE_PREFIX) + maxDigits + 1;
	char path[maxPathLength];
	for (int i = 1; i < num + 1; i++) {
		sprintf(path, "%s%s%d", MAX_DIR_PATH, MA_FILE_PREFIX, i);
		int fd = open(path, O_CREAT | O_RDWR, 0777);
		write(fd, "0", 1);
		close(fd);
	}
}

int countDigits(int n) {
	return (int) log10(n) + 1;
}

pid_t *startTrains(int num, char *mode) {
	int maxDigits = countDigits(num);
	pid_t *pids = malloc(sizeof(pid_t) * num);
	char id[maxDigits + 1];
	for (int i = 0; i < num; i++) {
		if ((pids[i] = fork()) == 0) {	//sono un figlio (di troia)
			sprintf(id, "%d", i);
			execl(TRAIN_PROCESS_PATH, TRAIN_PROCESS_NAME, id, mode, NULL);
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