#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define PATH_SEGMENT_DIR "../resources/MAx"
#define PATH_SEGMENT_FILE "/MA"
#define PATH_TRAIN_EXE "../bin/train"
#define NUMBER_OF_SEGMENTS 16
#define NUMBER_OF_TRAINS 5

unsigned short int countDecFigures(int);

unsigned short int countDecFigures(int n){
	unsigned result=0;
	if(n<0)n*=-1;
	while(n>0){
		result++;
		n/=10;
	}
	return result;
}

int main(int argc, char *argv[]) {

	struct stat st = {0};
	if(stat(PATH_SEGMENT_DIR,&st) == -1){	//crea directory se non esiste
		mkdir(PATH_SEGMENT_DIR, 0777);
	}
	umask(0000);
	for(int i=1;i<NUMBER_OF_SEGMENTS + 1;i++){	//crea i 16 nomi dei file di testo
		unsigned short const int segmentFigures=countDecFigures(i);
		char path[segmentFigures+strlen(PATH_SEGMENT_DIR)+strlen(PATH_SEGMENT_FILE)+1];
		sprintf(path,"%s%s%d", PATH_SEGMENT_DIR, PATH_SEGMENT_FILE, i);
		int fd = open(path, O_CREAT | O_RDWR, 0777);
		write(fd,"0",1);
		close(fd);
	}

	pid_t pids[NUMBER_OF_TRAINS];
	for(int i=0;i<NUMBER_OF_TRAINS;i++){
		if((pids[i] = fork()) == 0){	//sono un figlio
			unsigned short const int trainFigures=countDecFigures(i);
			char idTrain[trainFigures+1];
			sprintf(idTrain,"%d",i);
			execl(PATH_TRAIN_EXE,PATH_TRAIN_EXE,idTrain,argv[1],NULL);
		}
	}
	//questa parte di codice viene eseguita subito dal padre
	int status;
	pid_t pid;
	for(int i=NUMBER_OF_TRAINS; i>0;i--){
		pid=wait(&status);
		printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
	}
	return 0;
}