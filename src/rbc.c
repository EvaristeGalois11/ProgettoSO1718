#include "rbc.h"

int main(int argc, char *argv[]) {
	setUpExeDirPath(argv[0]);
	char *socketPath = csprintf("%s%s%s", exeDirPath, SOCKET_DIR_PATH, SOCKET_FILE_NAME);
	createDirIfNotExist(SOCKET_DIR_PATH);//questa funzione usa exeDirPath appena modificato e la macro da passare
	unlink(socketPath);
	printf("%s\n", "Rbc started!"); //?????????????????
	//int serverFd = make_name_socket(socketPath);
	//listen(serverFd, NUMBER_OF_TRAINS);
	while (1) {
		return 0;
	}
	return 0;
}

int make_name_socket(const char *filename) {
	struct sockaddr_un name;
	int sock;
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock < 0) {
		perror ("socket");
		exit (EXIT_FAILURE);
	}
	name.sun_family = AF_UNIX;
	strcpy(name.sun_path, filename);
	if (bind(sock, (struct sockaddr*)&name, sizeof(name)) < 0) {
		perror("bind");
		exit (EXIT_FAILURE);
	}
	return sock;
}