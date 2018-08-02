#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "common.h"

#define EXE_INFO_PATH "/proc/self/exe"
#define TRAIN_PROCESS_NAME "train"
#define NUMBER_OF_MA 16
#define NUMBER_OF_TRAINS 5
#define ETCS1 "ETCS1"
#define	ETCS2 "ETCS2"
#define RBC	"RBC"

static char *getExePath();
static void createDirIfNotExist(char *);
static void createMAxFiles(int);
static pid_t *startTrains(int, char *);
static void waitTrainsTermination(int);
static void launchETSC(char*);
static void launchRBC();
static void setUpSharedVariableForTrains();
static void cleanUpSharedVariableForTrains();

extern char *exeDirPath;
shared_data_trains *data_trains;

int main(int argc, char *argv[]) {
	exeDirPath = truncExeName(getExePath());

	if (argc == 2 && (strcasecmp(argv[1], ETCS1) == 0 || strcasecmp(argv[1], ETCS2) == 0)) {
		mode_t previousMask = umask(0000);
		createDirIfNotExist(MAX_DIR_PATH);
		createDirIfNotExist(LOG_DIR_PATH);
		createMAxFiles(NUMBER_OF_MA);
		launchETSC(argv[1]);
		umask(previousMask);
	} else if (argc == 3 && strcasecmp(argv[1], ETCS2) == 0 && strcasecmp(argv[2], RBC) == 0) {
		launchRBC();
	} else {
		printf("selezionare un modo valido per lanciare il programma ETCS1 | ETCS2 | ETCS2 RBC\n");
	}

	return 0;
}
void launchETSC(char *argv) {
	setUpSharedVariableForTrains();
	pid_t *pids = startTrains(NUMBER_OF_TRAINS, argv); //è veramente necessario registrare i pid dei treni?
	waitTrainsTermination(NUMBER_OF_TRAINS);
	free(pids);
	cleanUpSharedVariableForTrains();
}

void setUpSharedVariableForTrains() {
	int fd = shm_open(TRAIN_SHARED_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(fd, sizeof(shared_data_trains));
	data_trains = (shared_data_trains*) mmap(0, sizeof(shared_data_trains), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	pthread_mutexattr_t mutex_attr;
	pthread_mutexattr_init(&mutex_attr);
	pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&data_trains->mutex, &mutex_attr);
	pthread_mutexattr_destroy(&mutex_attr);

	pthread_condattr_t cond_attr;
	pthread_condattr_init(&cond_attr);
	pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
	pthread_cond_init(&data_trains->condvar, &cond_attr);
	pthread_condattr_destroy(&cond_attr);

	close(fd);
}

void cleanUpSharedVariableForTrains() {
	pthread_cond_destroy(&data_trains->condvar);
	pthread_mutex_destroy(&data_trains->mutex);
	munmap(data_trains, sizeof(shared_data_trains));
	shm_unlink(TRAIN_SHARED_NAME);
}

void launchRBC() {
	printf("stiamo avviando il server Kappa :)\n");
}

char *getExePath() {
	int size = 10;
	char *buffer = NULL;
	int ok = 0;
	while (ok == 0) {
		buffer = realloc(buffer, size);
		int nchars = readlink (EXE_INFO_PATH, buffer, size);
		if (nchars < 0) {
			free(buffer);
			perror("Impossible to read the exe path");
			exit(EXIT_FAILURE);
		} else if (nchars < size) {
			ok = 1;
		}
		size *= 2;
	}
	return buffer;
}

void createDirIfNotExist(char *dir) {
	char *pathExe = csprintf(exeDirPath, dir);
	struct stat st;
	if (stat(pathExe, &st) == -1) {
		mkdir(pathExe, 0777);
	}
	free(pathExe);
}

void createMAxFiles(int num) {
	for (int i = 1; i < num + 1; i++) {
		char *path = buildPathMAxFile(i);
		int fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0777);
		write(fd, "0", 1);
		close(fd);
		free(path);
	}
}

pid_t *startTrains(int num, char *mode) {
	int maxDigits = countDigits(num);
	pid_t *pids = malloc(sizeof(pid_t) * num);
	char id[maxDigits + 1];
	for (int i = 0; i < num; i++) {
		if ((pids[i] = fork()) == 0) {
			int len = strlen(exeDirPath) + strlen(TRAIN_PROCESS_NAME) + 1;
			char pathExe[len];
			sprintf(pathExe, "%s%s", exeDirPath, TRAIN_PROCESS_NAME); //FIXME se si usa csprintf dà come errore Permission denied
			sprintf(id, "%d", i + 1); //id numerato da 1 a 5
			execl(pathExe, pathExe, id, mode, NULL);
			perror("Train not started");
			exit(EXIT_FAILURE);
		}
	}
	return pids;
}

void waitTrainsTermination(int num) {
	int status;
	pid_t pid;
	for (int i = num; i > 0; i--) {
		pid = wait(&status);
		printf("Child with PID %ld exited with status 0x%x\n", (long) pid, status);
	}
}