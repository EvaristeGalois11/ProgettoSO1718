#include "rbc.h"

int main(int argc, char *argv[]) {
	setUpSharedVariable();
	setUpExeDirPath(argv[0]);
	createDirIfNotExist(SOCKET_DIR_PATH);
	importRoutes();
	startTrainSocket();
	cleanUp();
	return 0;
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

void importRoutes(void) {
	char *ertmsSocketAddr = buildPathErtmsSocketFile();
	int ertmsSocketFd = setUpSocket(ertmsSocketAddr, 1);
	listen(ertmsSocketFd, 1);
	int ertmsClientFd = accept(ertmsSocketFd, NULL, 0);
	for (int i = 0; i < NUMBER_OF_TRAINS; i++) {
		char *line = readLine(ertmsClientFd);
		printf("Rbc %d: Ricevuta linea: %s\n", tempId, line);
		routes[i] = generateRoute(line);
		starts[i] = routes[i];
		dataRbc -> stations[-(routes[i] -> id) - 1]++;
		free(line);
	}
	for (int i = 0; i < NUMBER_OF_STATIONS; i++) {
		printf("Rbc %d: Stazione %d: treni presenti %d\n", tempId, i, dataRbc -> stations[i]);
	}
	close(ertmsClientFd);
	close(ertmsSocketFd);
	unlink(ertmsSocketAddr);
}

void startTrainSocket(void) {
	char *trainSocketAddr = buildPathTrainSocketFile();
	int trainSocketFd = setUpSocket(trainSocketAddr, 1);
	listen(trainSocketFd, NUMBER_OF_TRAINS);
	for (int i = 0; i < NUMBER_OF_TRAINS; i++) {
		tempId = i;
		int clientFd = accept(trainSocketFd, NULL, 0);
		if (fork() == 0) {
			printf("Rbc %d, Accettata la connessione di un treno\n", tempId);
			serveTrain(clientFd);
			close(clientFd);
			cleanUp();
			return;
		}
		close(clientFd);
	}
	waitChildrenTermination(NUMBER_OF_TRAINS);
	close(trainSocketFd);
	unlink(trainSocketAddr);
	free(trainSocketAddr);
}

void serveTrain(int clientFd) {
	while (1) {
		if (waitForPosition(clientFd) < 0) {
			printf("Rbc %d: Connessione chiusa\n", tempId);
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
	printf("Rbc %d: Posizione comunicata %d\n", tempId, position);
	if (position > 0) {
		currLock = position - 1;
		printf("Rbc %d: Richiesto lock %d\n", tempId, currLock);
		pthread_mutex_lock(&(dataRbc -> mutexes[currLock]));
	}
	printf("Rbc %d: Invio risposta %s\n", tempId, OK);
	write(clientFd, OK, strlen(OK) + 1);
	free(str);
	return 0;
}

void waitForRequest(int clientFd) {
	char *str = readLine(clientFd);
	printf("Rbc %d: Ricevuta richiesta autorizzazione %s\n", tempId, str);
	int trainId, curr, next;
	sscanf(str, "%d,%d,%d", &trainId, &curr, &next);
	if (next > 0) {
		nextLock = next - 1;
		printf("Rbc %d: Richiesto lock %d\n", tempId, nextLock);
		pthread_mutex_lock(&dataRbc -> mutexes[nextLock]);
	}
	char *response;
	if ((routes[trainId - 1] -> next -> id == next) && (next < 0 || dataRbc -> ma[next - 1] == 0)) {
		updatePosition(trainId, curr, next);
		response = OK;
	} else {
		response = KO;
	}
	printf("Rbc %d: Invio risposta %s\n", tempId, response);
	logRbc(trainId, curr, next, response);
	write(clientFd, response, strlen(response) + 1);
	free(str);
}

void updatePosition(int trainId, int curr, int next) {
	//printf("Posizione treno %d: posizione %d\n", trainId, routes[trainId - 1] -> id);
	routes[trainId - 1] = routes[trainId - 1] -> next;
	dataRbc -> positions[trainId - 1]++;
	//printf("Posizione treno %d: posizione %d\n", trainId, routes[trainId - 1] -> id);
	if (curr < 0) {
		//printf("Stazione %d: numero treni %d\n", curr, dataRbc -> stations[-curr - 1]);
		dataRbc -> stations[-curr - 1]--;
		//printf("Stazione %d: numero treni %d\n", curr, dataRbc -> stations[-curr - 1]);
	} else {
		//printf("MA%d: stato %d\n", curr, dataRbc -> ma[curr - 1]);
		dataRbc -> ma[curr - 1] = 0;
		//printf("MA%d: stato %d\n", curr, dataRbc -> ma[curr - 1]);
	}
	if (next < 0) {
		//printf("Stazione %d: numero treni %d\n", next, dataRbc -> stations[-next - 1]);
		dataRbc -> stations[-next - 1]++;
		//printf("Stazione %d: numero treni %d\n", next, dataRbc -> stations[-next - 1]);
	} else {
		//printf("MA%d: stato %d\n", next, dataRbc -> ma[next - 1]);
		dataRbc -> ma[next - 1] = 1;
		//printf("MA%d: stato %d\n", next, dataRbc -> ma[next - 1]);
	}
}

void unlockMutex(int *id) {
	if (*id >= 0) {
		printf("Rbc %d: Sblocco lock %d\n", tempId, *id);
		pthread_mutex_unlock(&dataRbc -> mutexes[*id]);
	}
	*id = -1;
}