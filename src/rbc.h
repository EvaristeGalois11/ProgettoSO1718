#ifndef RBC_H
#define RBC_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "route.h"
#include "common.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h> //unlink()


int make_name_socket(const char*);

void setUpFileServerName(char*);
#endif