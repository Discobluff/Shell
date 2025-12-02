#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_JOBS 10

typedef struct {
    pid_t pid;
    char* command;
    int status; //-1 for no job, 0 for running, 1 for stopped, 2 for done
} job_t;

extern job_t jobs_list[MAX_JOBS];

int jobs(void);