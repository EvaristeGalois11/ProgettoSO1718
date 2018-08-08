#include "rbc.h"

int main(int argc, char *argv[]) {
	setUpSharedVariable();
	setUpExeDirPath(argv[0]);
	createDirIfNotExist(SOCKET_DIR_PATH);
	importRoutes();
	char *trainSocketAddr = buildPathTrainSocketFile();
	int trainSocketFd = setUpSocket(trainSocketAddr, 1);
	listen(trainSocketFd, NUMBER_OF_TRAINS);
	for (int i = 0; i < NUMBER_OF_TRAINS; i++) {
		int clientFd = accept(trainSocketFd, NULL, 0);
		if (fork() == 0) {
			printf("%s\n", "Accettata la connessione di un treno");
			startServeTrain(clientFd);
			close(clientFd);
			return 0;
		}
		close(clientFd);
	}
	waitChildrenTermination(NUMBER_OF_TRAINS);
	close(trainSocketFd);
	unlink(trainSocketAddr);
	free(trainSocketAddr);
	cleanUp();
	return 0;
}

void importRoutes(void) {
	char *ertmsSocketAddr = buildPathErtmsSocketFile();
	int ertmsSocketFd = setUpSocket(ertmsSocketAddr, 1);
	listen(ertmsSocketFd, 1);
	int ertmsClientFd = accept(ertmsSocketFd, NULL, 0);
	for (int i = 0; i < NUMBER_OF_TRAINS; i++) {
		char *line = readLine(ertmsClientFd);
		printf("Ricevuta linea: %s\n", line);
		routes[i] = generateRoute(line);
		starts[i] = routes[i];
		stations[-(routes[i] -> id) - 1]++;
		free(line);
	}
	for (int i = 0; i < NUMBER_OF_STATIONS; i++) {
		printf("Stazione %d: treni presenti %d\n", i, stations[i]);
	}
	close(ertmsClientFd);
	close(ertmsSocketFd);
	unlink(ertmsSocketAddr);
}

void setUpSharedVariable(void) {
	int fd = shm_open(RBC_SHARED_NAME, O_RDWR, 0700);
	dataRbc = (shared_data_rbc *) mmap(0, sizeof(shared_data_rbc), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
}

void cleanUp(void) {
	free(exeDirPath);
	for (int i = 0; i < NUMBER_OF_TRAINS; i++) {
		destroyRoute(starts[i]);
	}
	munmap(dataRbc, sizeof(shared_data_rbc));
	shm_unlink(RBC_SHARED_NAME);
}

void startServeTrain(int clientFd) {
	while (1) {
		if (waitForPosition(clientFd) < 0) {
			printf("%s\n", "Connessione chiusa");
			break;
		}
		waitForRequest(clientFd);
		unlockMutex(&currLock);
		unlockMutex(&nextLock);
	}
	close(clientFd);
}

int waitForPosition(int clientFd) {
	int position;
	char *str = readLine(clientFd);
	if (str == NULL) {
		return -1;
	}
	sscanf(str, "%d", &position);
	printf("Posizione comunicata %d\n", position);
	if (position > 0) {
		currLock = position - 1;
		printf("Richiesto lock %d\n", currLock);
		pthread_mutex_lock(&(dataRbc -> mutexes[currLock]));
	}
	printf("Invio risposta %s\n", OK);
	write(clientFd, OK, strlen(OK) + 1);
	free(str);
	return 0;
}

void waitForRequest(int clientFd) {
	char *str = readLine(clientFd);
	printf("Ricevuta richiesta autorizzazione %s\n", str);
	int trainId, curr, next;
	sscanf(str, "%d,%d,%d", &trainId, &curr, &next);
	nextLock = next - 1;
	printf("Richiesto lock %d\n", nextLock);
	pthread_mutex_lock(&dataRbc -> mutexes[nextLock]);
	char *response;
	if ((routes[trainId - 1] -> next -> id == next) && (next < 0 || ma[next - 1] == 0)) {
		updatePosition(trainId, curr, next);
		response = OK;
	} else {
		response = KO;
	}
	printf("Invio risposta %s\n", response);
	logRbc(trainId, curr, next, response);
	write(clientFd, response, strlen(response) + 1);
	free(str);
}

void updatePosition(int trainId, int curr, int next) {
	printf("Posizione treno %d: posizione %d\n", trainId, routes[trainId - 1] -> id);
	routes[trainId - 1] = routes[trainId - 1] -> next;
	printf("Posizione treno %d: posizione %d\n", trainId, routes[trainId - 1] -> id);
	if (curr < 0) {
		printf("Stazione %d: numero treni %d\n", curr, stations[-curr - 1]);
		stations[-curr - 1]--;
		printf("Stazione %d: numero treni %d\n", curr, stations[-curr - 1]);
	} else {
		printf("MA%d: stato %d\n", curr, ma[curr - 1]);
		ma[curr - 1] = 0;
		printf("MA%d: stato %d\n", curr, ma[curr - 1]);
	}
	if (next < 0) {
		printf("Stazione %d: numero treni %d\n", next, stations[-next - 1]);
		stations[-next - 1]++;
		printf("Stazione %d: numero treni %d\n", next, stations[-next - 1]);
	} else {
		printf("MA%d: stato %d\n", next, ma[next - 1]);
		ma[next - 1] = 1;
		printf("MA%d: stato %d\n", next, ma[next - 1]);
	}
}

void unlockMutex(int *id) {
	if (*id > 0) {
		printf("Sblocco lock %d\n", *id);
		pthread_mutex_unlock(&dataRbc -> mutexes[*id]);
	}
	*id = -1;
}