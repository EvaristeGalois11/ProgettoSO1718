#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "route.h"

#define SEGMENT_PREFIX "MA"
#define STATION_PREFIX "S"
#define CSV_SEPARATOR ","

static Node *buildNode(char *);
static char *readLine(char *);

Node *generateRoute(char *path) {
    char *line = readLine(path);
    char *saveState;
    char *token = strtok_r(line, CSV_SEPARATOR, &saveState);
    Node *start = NULL;
    Node *current;
    while (token != NULL) {
        if (start == NULL) {
            start = buildNode(token);
            current = start;
        } else {
            current -> next = buildNode(token);
            current = current -> next;
        }
        token = strtok_r(NULL, CSV_SEPARATOR, &saveState);
    }
    free(line);
    return start;
}

char *readLine(char *path) {
    FILE * file = fopen(path, "r");
    int CURR_MAX = 40;
    char ch = getc(file);
    char *buffer = malloc(sizeof(char) * CURR_MAX);
    int length = 0;
    while ((ch != '\n') && (ch != EOF)) {
        if(length == CURR_MAX) {
            CURR_MAX *= 2;
            buffer = realloc(buffer, CURR_MAX);
        }
        if (ch != ' ') {
            buffer[length] = ch;
            length++;
        }
        ch = getc(file);
    }
    if (length == CURR_MAX) {
        buffer = realloc(buffer, ++CURR_MAX);
    }
    buffer[length] = '\0';
    return buffer;
}


Node *buildNode(char *name) {
    Node *node = malloc(sizeof(Node));
    node -> next = NULL;
    if (strstr(name , SEGMENT_PREFIX) != NULL) {
        sscanf(name, SEGMENT_PREFIX"%d", &(node -> id));
    } else if (strstr(name , STATION_PREFIX) != NULL) {
        sscanf(name, STATION_PREFIX"%d", &(node -> id));
        node -> id *= -1;
    } else {
        fprintf(stderr, "%s is not a valid string!\n", name);
    }
    return node;	
}

int main(void) { // XXX solo per testing
    Node *temp = generateRoute("../resources/routes/T1");
    
    while (temp != NULL) {
        printf("id = %d\n", temp -> id);
        temp = temp -> next;
    }
    //char *temp = readLine("../resources/routes/T1");
    //printf("%s | %d\n", temp, strlen(temp));
    //free(temp);
    //printf("%d\n", decodify("MA2"));
    //printf("%d\n", decodify("S90293"));
    //printf("%d\n", decodify("pippo"));
}