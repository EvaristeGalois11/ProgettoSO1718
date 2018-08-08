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
	startTravel();
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
	char *trainSocketAddr = buildPathTrainSocketFile();
	clientFd = setUpSocket(trainSocketAddr, 0);
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

int checkMAxFile(int maId) {
	lockExclusiveMA(maId, &nextDescriptor);
	printf("Treno %d: Inizio lettura file: MA%d\n", trainId, maId);
	char byte;
	pread(nextDescriptor, &byte, 1, 0);
	printf("Treno %d: Fine lettura file: MA%d\n", trainId, maId);
	printf("Treno %d: Letto byte %c\n", trainId, byte);
	return (byte == '0');
}

int requestModeEtcs2(int train, int curr, int next) {
	char *message = csprintf("%d,%d,%d", trainId, curr, next);
	printf("Treno %d: Inviato messaggio %s\n", trainId, message);
	write(clientFd, message, strlen(message) + 1);
	char *response = readLine(clientFd);
	printf("Treno %d: Ricevuta risposta %s\n", trainId, response);
	int result = 0;
	if (strstr(response, OK)) {
		openFile(next, &nextDescriptor);
		result = 1;
	}
	free(message);
	free(response);
	return result;
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
		closeFile(&currDescriptor);
		closeFile(&nextDescriptor);
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
	if (openFile(maId, descriptor)) {
		printf("Treno %d: richiesto lock file MA%d\n", trainId, maId);
		fcntl (*descriptor, F_SETLKW, &writeLock);
		printf("Treno %d: ottenuto lock file MA%d\n", trainId, maId);
	}
}

int openFile(int maId, int *descriptor) {
	if (maId > 0) {
		char *path = buildPathMAxFile(maId);
		printf("Treno %d: aperto file MA%d\n", trainId, maId);
		*descriptor = open(path, O_RDWR);
		return 1;
	}
	return 0;
}

void closeFile(int *descriptor) {
	if (*descriptor != -1) {
		close(*descriptor);
		printf("Treno %d: chiusura di un file\n", trainId);
	}
	*descriptor = -1;
}

void notifyPosition(int maId, int *descriptor) {
	char *message = csprintf("%d", maId);
	printf("Treno %d: Notifica posizione %s\n", trainId, message);
	write(clientFd, message, strlen(message) + 1);
	openFile(maId, descriptor);
	char *response = readLine(clientFd);
	printf("Treno %d: Ricevuta risposta %s\n", trainId, response);
	free(response);
	free(message);
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