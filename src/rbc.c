#include "rbc.h"

int main(int argc, char *argv[]) {
	setUpSharedVariable();
	setUpExeDirPath(argv[0]);
	createDirIfNotExist(SOCKET_DIR_PATH);

	Node *routes;
	routes = importRoutes();

	char *socketAddr = csprintf("%s%s%s", exeDirPath, SOCKET_DIR_PATH, SOCKET_FILE_NAME);
	printf("%s\n", "Rbc started!");
	int socketFd = createSocket(socketAddr);
	listen(socketFd, NUMBER_OF_TRAINS);

	unlink(socketAddr);
	for (int i = 0; i < NUMBER_OF_TRAINS; i++) {
		int clientFd = accept(socketFd, NULL, 0);
		if (fork() == 0) {
			//startServeTrain(clientFd);
			char *str = readLine(clientFd);
			printf("%s\n", str);
			free(str);
			return 0;
		}
		close(clientFd);
	}
	waitChildrenTermination(NUMBER_OF_TRAINS);
	close(socketFd);
	unlink(socketAddr);
	cleanUp();
	return 0;
}

Node *importRoutes() {
	char *tempSocketAddr = csprintf("%s%s%s", exeDirPath, SOCKET_DIR_PATH, SOCKET_TEMP_NAME);
	int tempSocketFd = createSocket(tempSocketAddr);
	listen(tempSocketFd, 1);
	int tempClientFd = accept(tempSocketFd, NULL, 0);
	char *line;
	Node *node;
	for (int i = 0; i < NUMBER_OF_TRAINS; i++) {
		printf("XD\n");
		line = readLine(tempClientFd);
		printf("%s\n", line );
		node = generateRoute(line);
		printf("%d\n", i ); //NON ME LA STA STAMPANDO
	}
	printf("rofl\n" );
	free(line);
	free(node);
	close(tempClientFd);
	close(tempSocketFd);
	unlink(tempSocketAddr);
	return NULL;
}

void setUpSharedVariable(void) {
	int fd = shm_open(RBC_SHARED_NAME, O_RDWR, 0700);
	dataRbc = (shared_data_rbc *) mmap(0, sizeof(shared_data_rbc), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
}

void cleanUp(void) {
	free(exeDirPath);
	munmap(dataRbc, sizeof(shared_data_rbc));
	shm_unlink(RBC_SHARED_NAME);
}

int createSocket(const char *filename) {
	struct sockaddr_un name;
	int socketFd;
	socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (socketFd < 0) {
		perror("Impossible to obtain an anonymous socket");
		exit(EXIT_FAILURE);
	}
	name.sun_family = AF_UNIX;
	strcpy(name.sun_path, filename);
	if (bind(socketFd, (struct sockaddr *) &name, sizeof(name)) < 0) {
		perror("Impossible to bind the socket");
		exit(EXIT_FAILURE);
	}
	return socketFd;
}

void startServeTrain(int clientFd) {
	while (1) {
		if (waitForPosition(clientFd) < 0) {
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
	if (position > 0) {
		currLock = position;
		pthread_mutex_lock(&dataRbc -> mutexes[currLock]);
	}
	write(clientFd, OK, strlen(OK) + 1);
	free(str);
	return 0;
}

void waitForRequest(int clientFd) {
	char *str = readLine(clientFd);
	int trainId, curr, next;
	sscanf(str, "%d,%d,%d", &trainId, &curr, &next);
	nextLock = next;
	pthread_mutex_lock(&dataRbc -> mutexes[nextLock]);
	if (ma[nextLock]) {
		// TODO aggiornare posizione
		write(clientFd, OK, strlen(OK) + 1);
	} else {
		write(clientFd, KO, strlen(KO) + 1);
	}
	// TODO loggare richiesta e risposta
	free(str);
}