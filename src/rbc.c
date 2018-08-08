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
			printf("sono treno %d e questo il mio clientfd %d\n",i,clientFd );
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
		routes[i] = generateRoute(line);
		starts[i] = routes[i];
		stations[-(routes[i] -> id)]++;
		free(line);
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
		pthread_mutex_unlock(&dataRbc -> mutexes[currLock]);
		pthread_mutex_unlock(&dataRbc -> mutexes[nextLock]);
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
		currLock = position;
		pthread_mutex_lock(&dataRbc -> mutexes[currLock]);
	}
	//sleep(10);
	//write(clientFd, OK, strlen(OK) + 1);
	//perror("Write non riuscita");
	free(str);
	return 0;
}

void waitForRequest(int clientFd) {
	char *str = readLine(clientFd);
	int trainId, curr, next;
	sscanf(str, "%d,%d,%d", &trainId, &curr, &next);
	nextLock = next;
	pthread_mutex_lock(&dataRbc -> mutexes[nextLock]);
	char *response;
	if ((routes[trainId - 1] -> next -> id == next) && (next < 0 || ma[next])) {
		updatePosition(trainId, curr, next);
		response = OK;
	} else {
		response = KO;
	}
	logRbc(trainId, curr, next, response);
	write(clientFd, response, strlen(response) + 1);
	free(str);
}

void updatePosition(int trainId, int curr, int next) {
	routes[trainId - 1] = routes[trainId - 1] -> next;
	if (curr < 0) {
		stations[-curr]--;
	} else {
		ma[curr] = 0;
	}
	if (next < 0) {
		stations[-next]++;
	} else {
		ma[next] = 1;
	}
}