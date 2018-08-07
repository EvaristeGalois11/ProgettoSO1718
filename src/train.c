#include "train.h"

int main(int argc, char *argv[]) {
	setUpSharedVariable();
	setUpExeDirPath(argv[0]);
	sscanf(argv[1], "%d", &trainId);
	int isEtcs1 = (strcasecmp(argv[2], ETCS1) == 0);
	requestMode = isEtcs1 ? requestModeEtcs1 : requestModeEtcs2;
	lockMode = isEtcs1 ? lockExclusiveMA : notifyPosition;
	if (!isEtcs1) {
		connectToSocket();
	}
	start = readAndDecodeRoute();
	current = start;
	// startTravel();
	cleanUp();
	return 0;
}

void setUpSharedVariable(void) {
	int fd = shm_open(TRAIN_SHARED_NAME, O_RDWR, 0700);
	dataTrains = (shared_data_trains *) mmap(0, sizeof(shared_data_trains), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
}

void cleanUp(void) {
	free(exeDirPath);
	destroyRoute(start);
	close(clientFd);
	munmap(dataTrains, sizeof(shared_data_trains));
	shm_unlink(TRAIN_SHARED_NAME);
}

void connectToSocket(void) {
	struct sockaddr_un name;
	clientFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (clientFd < 0) {
		perror("Impossible to obtain an anonymous socket");
		exit(EXIT_FAILURE);
	}
	name.sun_family = AF_UNIX;
	char *socketAddr = csprintf("%s%s%s", exeDirPath, SOCKET_DIR_PATH, SOCKET_FILE_NAME);
	strcpy(name.sun_path, socketAddr);
	int result;
	do {
		result = connect(clientFd, (struct sockaddr *) &name, sizeof(name));
		if (result == -1) {
			sleep(SLEEP_TIME);
		}
	} while (result == -1);
	printf("%s\n", "connessi!");
	sleep(1);
	write(clientFd, "messaggiostandard\n", 20);
}

Node *readAndDecodeRoute(void) {
	char *path = buildPathRouteFile(trainId);
	int d = open(path, O_RDWR);
	char *line = readLine(d);
	Node *result = generateRoute(line);
	free(path);
	free(line);
	return result;
}

int requestModeEtcs1(int train, int curr, int next) {
	printf("Treno %d: richiesta autorizzazione file MA%d\n", trainId, next);
	if (next <= 0) {
		return 1;
	} else {
		return checkMAxFile(next);
	}
}

int checkMAxFile(int id) {
	lockExclusiveMA(id, &nextDescriptor);
	printf("Treno %d: Inizio lettura file: MA%d\n", trainId, id);
	char byte;
	pread(nextDescriptor, &byte, 1, 0);
	printf("Treno %d: Fine lettura file: MA%d\n", trainId, id);
	printf("Treno %d: letto byte %c\n", trainId, byte);
	return (byte == '0');
}

int requestModeEtcs2(int train, int curr, int next) {
	// TODO implementare richiesta al server RBC
	return 1;
}

void startTravel(void) {
	int count = 0;
	do {
		waitOtherTrains();
		lockMode(current -> id, &currDescriptor);
		waitOtherTrains();
		printf("Treno %d: giro numero %d\n", trainId, ++count);
		logTrain(trainId, current -> id, current -> next -> id);
		if (requestMode(trainId, current -> id, current -> next -> id)) {
			move();
			current = current -> next;
		}
		unlockFile(&currDescriptor);
		unlockFile(&nextDescriptor);
		printf("Treno %d: giro %d terminato\n", trainId, count);
		sleep(SLEEP_TIME);
	} while (current -> id > 0 && (current -> id != start -> id));
	logTrain(trainId, current -> id, 0);
	travelCompleted();
	printf("Treno %d: terminato\n", trainId);
}

void waitOtherTrains(void) {
	pthread_mutex_lock(&dataTrains -> mutex);
	checkOtherTrains(&dataTrains -> waiting, 1);
	pthread_mutex_unlock(&dataTrains -> mutex);
}

void checkOtherTrains(int *var, int notCompleted) {
	(*var)++;
	int numTrains = dataTrains -> waiting + dataTrains -> completed;
	printf("Treno %d: numTrains %d\n", trainId, numTrains);
	if (numTrains == NUMBER_OF_TRAINS) {
		printf("Treno %d: ci siamo tutti\n", trainId);
		eLUltimoChiudaLaPorta();
	} else if (notCompleted) {
		printf("Treno %d: manca ancora qualcuno\n", trainId);
		pthread_cond_wait(&dataTrains -> condvar, &dataTrains -> mutex);
	}
}

void eLUltimoChiudaLaPorta(void) {
	dataTrains -> waiting = 0;
	pthread_cond_broadcast(&dataTrains -> condvar);
}

void travelCompleted(void) {
	pthread_mutex_lock(&dataTrains -> mutex);
	checkOtherTrains(&dataTrains -> completed, 0);
	pthread_mutex_unlock(&dataTrains -> mutex);
}

void lockExclusiveMA(int maId, int *descriptor) {
	if (maId > 0) {
		char *path = buildPathMAxFile(maId);
		int d = open(path, O_RDWR);
		printf("Treno %d: richiesto lock file MA%d\n", trainId, maId);
		fcntl (d, F_SETLKW, &writeLock);
		printf("Treno %d: ottenuto lock file MA%d\n", trainId, maId);
		free(path);
		*descriptor = d;
	}
}

void unlockFile(int *descriptor) {
	if (*descriptor != -1) {
		close(*descriptor);
		printf("Treno %d: chiusura lock di un file\n", trainId);
	}
	*descriptor = -1;
}

void notifyPosition(int maId, int *descriptor) {
	// TODO notificare posizione corrente
}

void move(void) {
	if (currDescriptor != -1) {
		printf("Treno %d: Inizio scrittura file MA%d\n", trainId, (current -> id));
		writeOneByte(currDescriptor, "0");
		printf("Treno %d: Fine scrittura file MA%d\n", trainId, (current -> id));
	}
	if (nextDescriptor != -1) {
		printf("Treno %d: Inizio scrittura file MA%d\n", trainId, (current -> next -> id));
		writeOneByte(nextDescriptor, "1");
		printf("Treno %d: Fine scrittura file MA%d\n", trainId, (current -> next -> id));
	}
}

void writeOneByte(int descriptor, char *byte) {
	pwrite(descriptor, byte, 1, 0);
	fsync(descriptor);
}