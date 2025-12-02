#include "job.h"
#include <stdlib.h>
#include <stdio.h>

int jobs(void) {
    int no_job = 1;
    for (int i = 0; i < MAX_JOBS; i++) {
        //fprintf(stdout, "job status : %d\n", jobs_list[i].status);
        if (jobs_list[i].status != -1) {
            no_job = 0;
            const char* status_str;
            switch (jobs_list[i].status) {
                case 0:
                    status_str = "Running";
                    break;
                case 1:
                    status_str = "Stopped";
                    break;
                case 2:
                    status_str = "Done";
                    break;
                default:
                    status_str = "Unknown";
            }
            fprintf(stdout, "[%d] PID: %d, Command: %s, Status: %s\n", i + 1, jobs_list[i].pid, jobs_list[i].command, status_str);
        }
    }
    if (no_job) {
        fprintf(stdout, "No jobs running.\n");
    }

    return 0;
}

int remove_job(pid_t pid) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs_list[i].pid == pid) {
            free(jobs_list[i].command);
            jobs_list[i].pid = -1;
            jobs_list[i].command = NULL;
            jobs_list[i].status = -1; // no job running
            return 0; // success
        }
    }
    return -1; // job not found
}

