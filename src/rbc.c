#include "rbc.h"

int main(int argc, char *argv[]) {
	bool ma[16] = {0};	// TODO decidere come passare numero 16 di segmenti
	
	setUpFileServerName(argv[0]);
	unlink(exeDirPath);
	int serverFd = make_name_socket(exeDirPath);
	listen(serverFd,NUMBER_OF_TRAINS);
	while (1) {
		return 0;
	}

	return 0;
}

void setUpFileServerName(char *exePath){
	truncExeName(exePath);
	exeDirPath=csprintf("%s%s%s",exePath,FILE_SERVER_PATH,FILE_SERVER_NAME);

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