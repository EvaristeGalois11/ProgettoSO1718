#include "ertms.h"

int main(int argc, char *argv[]) {
	exeDirPath = truncExeName(getExePath());
	if (argc == 2 && (strcasecmp(argv[1], ETCS1) == 0 || strcasecmp(argv[1], ETCS2) == 0)) {
		mode_t previousMask = umask(0000);
		createDirIfNotExist(MAX_DIR_PATH);
		createDirIfNotExist(LOG_DIR_PATH);
		createMAxFiles();
		launchETSC(argv[1]);
		free(exeDirPath);
		umask(previousMask);
	} else if (argc == 3 && strcasecmp(argv[1], ETCS2) == 0 && strcasecmp(argv[2], RBC) == 0) {
		launchRBC();
	} else {
		printf("Invalid argument\n");
	}
	return 0;
}

char *getExePath(void) {
	int size = 15;
	char *buffer = NULL;
	int nchars = 0;
	do {
		size *= 2;
		buffer = realloc(buffer, size);
		nchars = readlink(EXE_INFO_PATH, buffer, size);
	} while (nchars == size);
	buffer[nchars] = '\0';
	return buffer;
}

void launchETSC(char *mode) {
	setUpSharedVariableForTrains();
	startTrains(mode);
	waitTrainsTermination();
	cleanUpSharedVariableForTrains();
}

void setUpSharedVariableForTrains(void) {
	int fd = shm_open(TRAIN_SHARED_NAME, O_CREAT | O_TRUNC | O_RDWR, 0700);
	ftruncate(fd, sizeof(shared_data_trains));
	dataTrains = (shared_data_trains*) mmap(0, sizeof(shared_data_trains), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	pthread_mutexattr_t mutex_attr;
	pthread_mutexattr_init(&mutex_attr);
	pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&dataTrains -> mutex, &mutex_attr);
	pthread_mutexattr_destroy(&mutex_attr);
	pthread_condattr_t cond_attr;
	pthread_condattr_init(&cond_attr);
	pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
	pthread_cond_init(&dataTrains -> condvar, &cond_attr);
	pthread_condattr_destroy(&cond_attr);
	close(fd);
}

void cleanUpSharedVariableForTrains(void) {
	pthread_cond_destroy(&dataTrains -> condvar);
	pthread_mutex_destroy(&dataTrains -> mutex);
	munmap(dataTrains, sizeof(shared_data_trains));
	shm_unlink(TRAIN_SHARED_NAME);
}

void launchRBC(void) {
	// TODO implementare avvio RBC
}

void createDirIfNotExist(char *dir) {
	char *pathDir = csprintf("%s%s", exeDirPath, dir);
	struct stat st;
	if (stat(pathDir, &st) == -1) {
		mkdir(pathDir, 0777);
	}
	free(pathDir);
}

void createMAxFiles() {
	for (int i = 1; i < NUMBER_OF_MA + 1; i++) {
		char *path = buildPathMAxFile(i);
		int fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0777);
		write(fd, "0", 1);
		close(fd);
		free(path);
	}
}

void startTrains(char *mode) {
	for (int i = 1; i < NUMBER_OF_TRAINS + 1; i++) {
		if (fork() == 0) {
			char *pathTrain = csprintf("%s%s", exeDirPath, TRAIN_PROCESS_NAME);
			char *id = csprintf("%d", i);
			execl(pathTrain, pathTrain, id, mode, NULL);
			perror("Train not started");
			exit(EXIT_FAILURE);
		}
	}
}

void waitTrainsTermination() {
	int status;
	pid_t pid;
	for (int i = 0; i < NUMBER_OF_TRAINS; i++) {
		pid = wait(&status);
		printf("Child with PID %ld exited with status 0x%x\n", (long) pid, status);
	}
}