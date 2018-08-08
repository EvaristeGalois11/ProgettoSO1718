#include "common.h"

char *buildPathMAxFile(int id) {
	if (id > 0) {
		return csprintf("%s%s%s%d", exeDirPath, MAX_DIR_PATH, MA_FILE_PREFIX, id);
	} else {
		return NULL;
	}
}

char *buildPathRouteFile(int id) {
	return csprintf("%s%s%s%d", exeDirPath, ROUTES_DIR_PATH, ROUTE_FILE_PREFIX, id);
}

char *buildPathTrainLogFile(int id) {
	return csprintf("%s%s%s%d%s", exeDirPath, LOG_DIR_PATH, LOG_TRAIN_FILE_PREFIX, id, LOG_EXTENSION);
}

char *buildPathRbcLogFile(void) {
	return csprintf("%s%s%s%s", exeDirPath, LOG_DIR_PATH, LOG_RBC_FILE_PREFIX, LOG_EXTENSION);
}

char *buildPathTrainSocketFile(void) {
	return csprintf("%s%s%s", exeDirPath, SOCKET_DIR_PATH, TRAIN_SOCKET_NAME);
}

char *buildPathErtmsSocketFile(void) {
	return csprintf("%s%s%s", exeDirPath, SOCKET_DIR_PATH, ERTMS_SOCKET_NAME);
}

char *csprintf(const char *format, ...) {
	va_list arglist1, arglist2;
	va_start(arglist1, format);
	va_copy(arglist2, arglist1);
	int needed = vsnprintf(NULL, 0, format, arglist1);
	char *string = malloc(needed + 1);
	vsprintf(string, format, arglist2);
	va_end(arglist1);
	va_end(arglist2);
	return string;
}

void setUpExeDirPath(char *exePath) {
	exeDirPath = csprintf("%s", exePath);
	truncExeName(exeDirPath);
}

char *truncExeName(char *exePath) {
	char *lastOccurance = NULL;
	char *temp = exePath;
	do {
		lastOccurance = temp;
		temp = strstr(++lastOccurance, "/");
	} while (temp != NULL);
	lastOccurance[0] = '\0';
	return exePath;
}

void createDirIfNotExist(char *dir) {
	char *pathDir = csprintf("%s%s", exeDirPath, dir);
	struct stat st;
	if (stat(pathDir, &st) == -1) {
		mkdir(pathDir, 0777);
	}
	free(pathDir);
}

void waitChildrenTermination(int numChild) {
	int status;
	pid_t pid;
	for (int i = 0; i < numChild; i++) {
		pid = wait(&status);
		printf("Child with PID %ld exited with status %d\n", (long) pid, status);
	}
}

int setUpSocket(char *filename, int isServer) {
	struct sockaddr_un name;
	int socketFd;
	socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (socketFd < 0) {
		perror("Impossible to obtain an anonymous socket");
		exit(EXIT_FAILURE);
	}
	name.sun_family = AF_UNIX;
	strcpy(name.sun_path, filename);
	if (isServer) {
		unlink(filename);
		if (bind(socketFd, (struct sockaddr *) &name, sizeof(name)) < 0) {
			perror("Impossible to bind the socket");
			exit(EXIT_FAILURE);
		}
	} else {
		int result;
		do {
			result = connect(socketFd, (struct sockaddr *) &name, sizeof(name));
			if (result == -1) {
				sleep(1);
			}
		} while (result == -1);
	}
	return socketFd;
}