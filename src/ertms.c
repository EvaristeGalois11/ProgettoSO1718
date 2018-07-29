#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define PATH_SEGMENT_DIR "../resources/MAx"
#define PATH_SEGMENT_FILE "/MA"
#define PATH_TRAIN_EXE "../bin/train"
#define NUMBER_OF_SEGMENTS 16
#define NUMBER_OF_TRAINS 5

int main(int argc, char *argv[]) {

	struct stat st = {0};
	if(stat(PATH_SEGMENT_DIR,&st) == -1){	//crea directory se non esiste
		mkdir(PATH_SEGMENT_DIR, 0777);
	}
	umask(0000);
	char *path=(char*)malloc(10*sizeof(char));
	for(int i=1;i<NUMBER_OF_SEGMENTS + 1;i++){	//crea i 16 nomi dei file di testo
		sprintf(path,"%s%s%d", PATH_SEGMENT_DIR, PATH_SEGMENT_FILE, i);
		int fd = open(path, O_CREAT | O_RDWR, 0777);
		write(fd,"0",1);
		close(fd);
	}
	free(path);
	path=NULL;

	pid_t pids[NUMBER_OF_TRAINS];
	for(int i=0;i<NUMBER_OF_TRAINS;i++){
		if((pids[i] = fork()) == 0){	//sono un figlio
			//exe
			char idTrain[12];
			sprintf(idTrain,"%d",i);
			execl(PATH_TRAIN_EXE,PATH_TRAIN_EXE,idTrain,argv[1],NULL);
		}
	}
	//questa parte di codice viene eseguita subito dal padre
	int rofl;
	scanf("%d",&rofl);
	return 0;
}