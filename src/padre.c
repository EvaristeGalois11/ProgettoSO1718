#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <malloc.h>
#define PATHSTART "./MAx/MA"
#define NUMBER_OF_SEGMENTS 16
#define ALLPRIVILEGES 0777

int main(int argc, char *argv[]) {

	struct stat st = {0};
	if(stat("./MAx",&st) == -1){	//crea directory se non esiste
		mkdir("./MAx", ALLPRIVILEGES);
	}
	umask(0000);
	char *path=(char*)malloc(10*sizeof(char));
	for(int i=1;i<NUMBER_OF_SEGMENTS + 1;i++){		//crea i 16 nomi dei file di testo
		sprintf(path,"%s%d", PATHSTART, i);
		int fd = open(path, O_CREAT | O_RDWR, ALLPRIVILEGES);
		write(fd,"0",1);
		close(fd);
	}
	free(path);
	path=NULL;
	return 0;
}