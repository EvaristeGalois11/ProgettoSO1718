#include "train.h"

int main(int argc, char *argv[]) {
	setUpSharedVariable();
	setUpExeDirPath(argv[0]);
	trainId = toInt(argv[1]);
	requestMode = (strcmp(argv[2], ETCS1) == 0) ? requestModeEtcs1 : requestModeEtcs2;
	current = readAndDecodeRoute();
	startTravel();
	free(exeDirPath);
	cleanUpSharedVariable();
	return 0;
}

void setUpSharedVariable() {
	int fd = shm_open(TRAIN_SHARED_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	data_trains = (shared_data_trains*) mmap(0, sizeof(shared_data_trains), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
}

void cleanUpSharedVariable() {
	munmap(data_trains, sizeof(shared_data_trains));
	shm_unlink(TRAIN_SHARED_NAME);
}

void setUpExeDirPath(char *exePath) {
	char *temp = malloc(sizeof(char) * strlen(exePath) + 1);
	strcpy(temp, exePath);
	exeDirPath = truncExeName(temp);
}

Node *readAndDecodeRoute() {
	char *path = buildPathRouteFile(trainId);
	Node *result = generateRoute(path);
	free(path);
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

void startTravel() {
	int count = 0;
	do {
		waitOtherTrains();
		lockExclusiveMA(current -> id, &currDescriptor);
		waitOtherTrains();
		printf("Treno %d: giro numero %d\n", trainId, ++count);
		logTrain(trainId, current -> id, current -> next -> id);
		if (requestModeEtcs1(trainId, current -> id, current -> next -> id)) {
			move();
			current = current -> next;
		}
		unlockFile(&currDescriptor);
		unlockFile(&nextDescriptor);
		printf("Treno %d: giro %d terminato\n", trainId, count);
		sleep(SLEEP_TIME);
	} while (current -> id > 0);
	logTrain(trainId, current -> id, 0);
	travelCompleted();
	printf("Treno %d: terminato\n", trainId);
}

void waitOtherTrains() {
	pthread_mutex_lock(&data_trains -> mutex);
	int numTrains = data_trains -> waiting + data_trains -> completed;
	printf("Treno %d: numTrains in waitOtherTrains %d\n", trainId, numTrains);
	if (numTrains != NUMBER_OF_TRAINS - 1) {
		printf("Treno %d: manca ancora qualcuno\n", trainId);
		data_trains -> waiting++;
		pthread_cond_wait(&data_trains -> condvar, &data_trains -> mutex);
	} else if (numTrains == NUMBER_OF_TRAINS - 1) {
		printf("Treno %d: ci siamo tutti\n", numTrains);
		eLUltimoChiudaLaPorta();
	}
	pthread_mutex_unlock(&data_trains -> mutex);
}

void eLUltimoChiudaLaPorta() {
	data_trains -> waiting = 0;
	pthread_cond_broadcast(&data_trains -> condvar);
}

void travelCompleted() {
	pthread_mutex_lock(&data_trains->mutex);
	data_trains -> completed++;
	int numTrains = data_trains -> waiting + data_trains -> completed;
	printf("Treno %d: numTrains in travelCompleted %d\n", trainId, numTrains);
	if (numTrains == NUMBER_OF_TRAINS) {
		printf("Treno %d: %s\n", trainId, "ci siamo tutti");
		eLUltimoChiudaLaPorta();
	}
	pthread_mutex_unlock(&data_trains -> mutex);
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

void move() {
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

