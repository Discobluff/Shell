#ifndef RUNNER_H
#define RUNNER_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>

#define MAX_ARG_SIZE 32

extern int debug;

int run(char** line);

#endif
