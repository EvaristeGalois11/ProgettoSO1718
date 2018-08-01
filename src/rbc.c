#include <stdio.h>
#include <stdbool.h>
#include "route.h"
#include "common.h"

Node **importaPercorsiTreni(char[]);
void creaServer();
void listen();

int main(int argc, char *argv[]){
	bool ma[16]={0};	// TODO decidere come passare numero 16 di segmenti
	Node **percorsiTreni;
	percorsiTreni=importaPercorsiTreni(argv[1]);

	creaServer();
	while(1){	//viene interrotto dal chiamante ermst
		listen();
		return 0;
	}

	return 0;
}

Node** importaPercorsiTreni(char argv[]){
	// TODO incrociare i flussi stream come i ghostbusters
	return NULL;
}

void creaServer(){
	return;
}

void listen(){
	return;
}