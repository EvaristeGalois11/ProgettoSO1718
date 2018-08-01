#include <stdio.h>
#include "route.h"
#include "common.h"

void creaStrutturaDatiSegmenti();
Node** importaPercorsiTreni(char[]);
void creaServer();
void listen();

int main(int argc, char *argv[]){
	creaStrutturaDatiSegmenti();
	Node *percorsiTreni[5];
	percorsiTreni=importaPercorsiTreni(argv[1]);

	creaServer();
	while(1){	//viene interrotto dal chiamante ermst
		listen();
		return 0; //per ora
	}

	return 0;
}

void creaStrutturaDatiSegmenti(){
	return;
}

Node** importaPercorsiTreni(char argv[]){
	return NULL;
}

void creaServer(){
	return;
}

void listen(){
	return;
}