#ifndef RUNNER_H
#define RUNNER_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

int run(char** args);

#endif
