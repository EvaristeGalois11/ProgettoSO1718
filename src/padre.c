#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#define PATHSTART "./MAx/MA"
#define PATHEND ".txt"

int main(int argc, char *argv[]) {

	struct stat st = {0};
	if(stat("./MAx",&st) == -1){	//crea directory se non esiste
		mkdir("./MAx", 0777);
	}

	for(int i=1;i<17;i++){		//crea i 16 nomi dei file di testo
		char path[15]=PATHSTART;
		int temp=i;
		if(temp>9){
			path[8]='1';
			path[9]=(temp-10)+'0';
		}else{
			path[8]=temp+'0';
		}
		
		int fd = open(strcat(path, PATHEND), O_CREAT | O_RDWR, 0777);
	}
	/*int fd;
	fd=open("./MAx/MA1.txt", O_RDWR | O_CREAT, 0777);
	if (fd==-1){
		printf("roflxd\n");
	}*/
	return 0;
	

}