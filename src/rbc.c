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
		routes[i] = generateRoute(line);
		starts[i] = routes[i];
		dataRbc -> stations[-(routes[i] -> id) - 1]++;
		free(line);
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
	if (position > 0) {
		currLock = position - 1;
		pthread_mutex_lock(&(dataRbc -> mutexes[currLock]));
	}
	write(clientFd, OK, strlen(OK) + 1);
	free(str);
	return 0;
}

void waitForRequest(int clientFd) {
	char *str = readLine(clientFd);
	int trainId, curr, next;
	sscanf(str, "%d,%d,%d", &trainId, &curr, &next);
	if (next > 0) {
		nextLock = next - 1;
		pthread_mutex_lock(&dataRbc -> mutexes[nextLock]);
	}
	char *response;
	if ((routes[trainId - 1] -> next -> id == next) && (next < 0 || dataRbc -> ma[next - 1] == 0)) {
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
	dataRbc -> positions[trainId - 1]++;
	if (curr < 0) {
		dataRbc -> stations[-curr - 1]--;
	} else {
		dataRbc -> ma[curr - 1] = 0;
	}
	if (next < 0) {
		dataRbc -> stations[-next - 1]++;
	} else {
		dataRbc -> ma[next - 1] = 1;
	}
}

void unlockMutex(int *id) {
	if (*id >= 0) {
		pthread_mutex_unlock(&dataRbc -> mutexes[*id]);
	}
	*id = -1;
}