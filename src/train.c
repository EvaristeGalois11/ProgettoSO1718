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
	if (clientFd != -1) {
		close(clientFd);
	}
	munmap(dataTrains, sizeof(shared_data_trains));
	shm_unlink(TRAIN_SHARED_NAME);
}

void connectToSocket(void) {
	char *trainSocketAddr = buildPathTrainSocketFile();
	clientFd = setUpSocket(trainSocketAddr, 0);
	free(trainSocketAddr);
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
	if (next <= 0) {
		return 1;
	} else {
		return checkMAxFile(next);
	}
}

int checkMAxFile(int maId) {
	lockExclusiveMA(maId, &nextDescriptor);
	char byte;
	pread(nextDescriptor, &byte, 1, 0);
	return (byte == '0');
}

int requestModeEtcs2(int train, int curr, int next) {
	char *message = csprintf("%d,%d,%d", trainId, curr, next);
	write(clientFd, message, strlen(message) + 1);
	char *response = readLine(clientFd);
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
	do {
		waitOtherTrains();
		lockMode(current -> id, &currDescriptor);
		waitOtherTrains();
		logTrain(trainId, current -> id, current -> next -> id);
		if (requestMode(trainId, current -> id, current -> next -> id)) {
			move();
			current = current -> next;
		}
		closeFile(&currDescriptor);
		closeFile(&nextDescriptor);
		sleep(SLEEP_TIME);
	} while (current -> id > 0 && (current -> id != start -> id));
	logTrain(trainId, current -> id, 0);
	travelCompleted();
}

void waitOtherTrains(void) {
	pthread_mutex_lock(&dataTrains -> mutex);
	checkOtherTrains(&dataTrains -> waiting, 1);
	pthread_mutex_unlock(&dataTrains -> mutex);
}

void checkOtherTrains(int *var, int notCompleted) {
	(*var)++;
	int numTrains = dataTrains -> waiting + dataTrains -> completed;
	if (numTrains == NUMBER_OF_TRAINS) {
		eLUltimoChiudaLaPorta();
	} else if (notCompleted) {
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
		fcntl(*descriptor, F_SETLKW, &writeLock);
	}
}

int openFile(int maId, int *descriptor) {
	if (maId > 0) {
		char *path = buildPathMAxFile(maId);
		*descriptor = open(path, O_RDWR);
		free(path);
		return 1;
	}
	return 0;
}

void closeFile(int *descriptor) {
	if (*descriptor != -1) {
		close(*descriptor);
	}
	*descriptor = -1;
}

void notifyPosition(int maId, int *descriptor) {
	char *message = csprintf("%d", maId);
	write(clientFd, message, strlen(message) + 1);
	openFile(maId, descriptor);
	char *response = readLine(clientFd);
	free(response);
	free(message);
}

void move(void) {
	if (currDescriptor != -1) {
		writeOneByte(currDescriptor, "0");
	}
	if (nextDescriptor != -1) {
		writeOneByte(nextDescriptor, "1");
	}
}

void writeOneByte(int descriptor, char *byte) {
	pwrite(descriptor, byte, 1, 0);
	fsync(descriptor);
}