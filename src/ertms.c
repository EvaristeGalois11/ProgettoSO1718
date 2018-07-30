#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

#define EXE_INFO_PATH "/proc/self/exe"
#define MAX_DIR_PATH "../resources/MAx"
#define MA_FILE_PREFIX "/MA"
#define TRAIN_PROCESS_NAME "train"
#define NUMBER_OF_MA 16
#define NUMBER_OF_TRAINS 5

static void createDirIfNotExist(char *);
static void createMAxFiles(int);
static int countDigits(int);
static pid_t *startTrains(int, char *);
static void waitTrainsTermination();
static char *getExePath(char*);
static char *truncExeName(char*);


static char *exeDirPath;

int main(int argc, char *argv[]) {
	exeDirPath=truncExeName(getExePath(EXE_INFO_PATH));
	mode_t previousMask = umask(0000);
	createDirIfNotExist(MAX_DIR_PATH);
	createMAxFiles(NUMBER_OF_MA);
	pid_t *pids = startTrains(NUMBER_OF_TRAINS, argv[1]);
	waitTrainsTermination();
	free(pids);
	umask(previousMask);
	return 0;
}

char *getExePath(char *filename){
	int size=10;
	char *buffer=NULL;
	while(1){
		buffer= realloc(buffer, size);
		int nchars= readlink (filename, buffer, size);
		if(nchars<0){ //errore se nchars=-1
			free(buffer);
			return NULL;
		}
		else if(nchars < size)return buffer;
		else size*=2; //se nchars==size il filename e' stato troncato e ha bisogno di più spazio
	}
}

char *truncExeName(char *exePath){
	int len=strlen(exePath);
	while(exePath[len]!='/' && len>=0){
		len--;
		}
	if(len==0)return exePath; //errore nel path, non modificare
	len+=1;	//len+1 per il terminatore
	exePath=realloc(exePath,len);
	exePath[len]='\0';
	return exePath;
}

void createDirIfNotExist(char *dir) {
	struct stat st;
	const unsigned int len=strlen(exeDirPath)+strlen(dir)+1;
	char pathExe[len];
	strcpy(pathExe,exeDirPath);
	strcat(pathExe,dir);
	if (stat(pathExe, &st) == -1) {
		mkdir(pathExe, 0777);
	}
}

void createMAxFiles(int num) {
	int maxDigits = countDigits(num);
	int maxPathLength = strlen(exeDirPath) + strlen(MAX_DIR_PATH) + strlen(MA_FILE_PREFIX) + maxDigits + 1;
	char path[maxPathLength];
	for (int i = 1; i < num + 1; i++) {
		sprintf(path, "%s%s%s%d",exeDirPath, MAX_DIR_PATH, MA_FILE_PREFIX, i);
		int fd = open(path, O_CREAT | O_RDWR, 0777);
		write(fd, "0", 1);
		close(fd);
	}
}

int countDigits(int n) {
	return (int) log10(n) + 1;
}

pid_t *startTrains(int num, char *mode) {
	int maxDigits = countDigits(num);
	pid_t *pids = malloc(sizeof(pid_t) * num);
	char id[maxDigits + 1];
	for (int i = 0; i < num; i++) {
		if ((pids[i] = fork()) == 0) {	//sono un figlio (di troia)
			const unsigned int len=strlen(exeDirPath)+strlen(TRAIN_PROCESS_NAME)+1;
			char pathExe[len];
			sprintf(pathExe,"%s%s",exeDirPath,TRAIN_PROCESS_NAME);
			sprintf(id, "%d", i);
			execl(pathExe, TRAIN_PROCESS_NAME, id, mode, NULL);
			perror("Child");
			exit(errno);
		}
	}
	return pids;
}

void waitTrainsTermination() {
	int status;
	pid_t pid;
	for (int i = NUMBER_OF_TRAINS; i > 0; i--) {
		pid = wait(&status);
		printf("Child with PID %ld exited with status 0x%x\n", (long) pid, status);
	}
}