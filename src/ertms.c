#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "ertms.h"

#include "route.h"

int main(int argc, char *argv[]) {
	mode_t previousMask = umask(0000);
	exeDirPath = truncExeName(getExePath());
	if (argc == 2 && (strcasecmp(argv[1], ETCS1) == 0 || strcasecmp(argv[1], ETCS2) == 0)) {
		createDirIfNotExist(MAX_DIR_PATH);
		createDirIfNotExist(LOG_DIR_PATH);
		createMAxFiles();
		launchEtsc(argv[1]);
	} else if (argc == 3 && strcasecmp(argv[1], ETCS2) == 0 && strcasecmp(argv[2], RBC) == 0) {
		launchRbc();
	} else {
		printf("Invalid argument\n");
	}
	free(exeDirPath);
	umask(previousMask);
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

void launchEtsc(char *mode) {
	setUpSharedVariableForTrains();
	startTrains(mode);
	waitChildrenTermination(NUMBER_OF_TRAINS);
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

void launchRbc(void) {
	setUpSharedVariableForRbc();
	startRbc();
	//
	struct sockaddr_un name;
	int clientFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (clientFd < 0) {
		perror("Impossible to obtain an anonymous socket");
		exit(EXIT_FAILURE);
	}
	name.sun_family = AF_UNIX;
	char *socketAddr = csprintf("%s%s%s", exeDirPath, SOCKET_DIR_PATH, SOCKET_TEMP_NAME);
	strcpy(name.sun_path, socketAddr);
	int result;
	do {
		result = connect(clientFd, (struct sockaddr *) &name, sizeof(name));
		if (result == -1) {
			sleep(1);
		}
	} while (result == -1);
	printf("%s\n", "connessi!");
	sleep(1);//SERVE A QUALCOSA O È A CASO?

	for (int i = 1; i < NUMBER_OF_TRAINS + 1; i++) {
		char *path = buildPathRouteFile(i);
		int d = open(path, O_RDWR);
		char *line = readLine(d);
		write(clientFd, line, strlen(line) + 1);
		free(path);
		free(line);
	}
	//SPUDORATAMENTE COPIATO DA TRAIN PER PROVARE
	waitChildrenTermination(1);//aspetta che muoia il server
	cleanUpSharedVariableForRbc();
}

void setUpSharedVariableForRbc(void) {
	int fd = shm_open(RBC_SHARED_NAME, O_CREAT | O_TRUNC | O_RDWR, 0700);
	ftruncate(fd, sizeof(shared_data_rbc));
	dataRbc = (shared_data_rbc*) mmap(0, sizeof(shared_data_rbc), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	pthread_mutexattr_t mutex_attr;
	pthread_mutexattr_init(&mutex_attr);
	pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
	for (int i = 0; i < NUMBER_OF_MA; i++) {
		pthread_mutex_init(&dataRbc -> mutexes[i], &mutex_attr);
	}
	pthread_mutexattr_destroy(&mutex_attr);
	close(fd);
}

void cleanUpSharedVariableForRbc(void) {
	for (int i = 0; i < NUMBER_OF_MA; i++) {
		pthread_mutex_destroy(&dataRbc -> mutexes[i]);
	}
	munmap(dataRbc, sizeof(shared_data_rbc));
	shm_unlink(RBC_SHARED_NAME);
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

void startRbc(void) {
	if (fork() == 0) {
		char *pathRbc = csprintf("%s%s", exeDirPath, RBC_PROCESS_NAME);
		execl(pathRbc, pathRbc, NULL);
		perror("Rbc not started");
		exit(EXIT_FAILURE);
	}
	printf("ciao sono papà\n");

}