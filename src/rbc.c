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

	for (int i = 0; i < NUMBER_OF_TRAINS; i++) {
		int clienFd = accept(socketFd, NULL, 0);
		if (fork() == 0) {
			char junk[20];
			if (read(clienFd, junk, 20) == 0) {
				printf("%s\n", "client closed connection");
				return 0;
			}
			printf("%s\n", junk);
			close(clienFd);
			return 0;
		}
		close(clienFd);
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
	char junk[20];
	read(tempClientFd, junk, 20);
	printf("%s\n", junk );
	close(tempClientFd);
	close(tempSocketFd);
	unlink(tempSocketAddr);
	return NULL;
}

void setUpSharedVariable(void) {
	int fd = shm_open(RBC_SHARED_NAME, O_RDWR, 0700);
	data_rbc = (shared_data_rbc *) mmap(0, sizeof(shared_data_rbc), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
}

void cleanUp(void) {
	free(exeDirPath);
	munmap(data_rbc, sizeof(shared_data_rbc));
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