#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "route.h"
#include "common.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h> //unlink()

Node **importaPercorsiTreni(char[]);
void creaServer();

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
int main(int argc, char *argv[]) {
	bool ma[16] = {0};	// TODO decidere come passare numero 16 di segmenti
	Node **percorsiTreni;
	percorsiTreni = importaPercorsiTreni(argv[1]);

	creaServer();
	unlink("./serverFdFile");
	int serverFd = make_name_socket("./serverFdFile");
	listen(serverFd,NUMBER_OF_TRAINS);
	while (1) {
		return 0;
	}

	return 0;
}

Node** importaPercorsiTreni(char argv[]) {
	// TODO incrociare i flussi stream come i ghostbusters
	return NULL;
}

void creaServer() {
	return;
}