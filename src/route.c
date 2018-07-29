#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "route.h"

#define SEGMENT_PREFIX "MA"
#define STATION_PREFIX "S"

static int decodify(char *);
static char *readLine(char *);

Node generateRoute(char *string) {
}

char *readLine(char *path) {
    FILE * file = fopen(path, "r");

    int CUR_MAX = 20;
    char ch = getc(file);
    char *buffer = (char*) malloc(sizeof(char) * CUR_MAX);
    int length = 0;

    while ((ch != '\n') && (ch != EOF)) {
        if(length == CUR_MAX) {
            CUR_MAX *= 2;
            buffer = realloc(buffer, CUR_MAX);
        }
        if (ch != ' ') {
            buffer[length] = ch;
            length++;
        }
        ch = getc(file);
    }
    buffer[length] = '\0';
    return buffer;
}


int decodify(char *name) {
    int id = 0;
    if (strstr(name , SEGMENT_PREFIX) != NULL) {
        sscanf(name, SEGMENT_PREFIX"%d", &id);
    } else if (strstr(name , STATION_PREFIX) != NULL) {
        sscanf(name, STATION_PREFIX"%d", &id);
    } else {
        fprintf(stderr, "%s is not a valid string!\n", name);
    }
    return id;	
}

int main(void) { // solo per testing
    printf("%s", readLine("../resources/routes/T1"));
    //printf("%d\n", decodify("MA2"));
    //printf("%d\n", decodify("S90293"));
    //printf("%d\n", decodify("pippo"));
}