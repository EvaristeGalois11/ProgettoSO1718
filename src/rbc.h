#ifndef RBC_H
#define RBC_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include "route.h"
#include "common.h"

int make_name_socket(const char*);
void setUpFileServerName(char*);
#endif