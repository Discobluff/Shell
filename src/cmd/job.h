#include <stdlib.h>

#define MAX_JOBS 10

typedef struct {
    pid_t pid;
    char* command;
    int status;
} job_t;

job_t jobs_list[MAX_JOBS];