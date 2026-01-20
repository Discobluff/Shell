#define _XOPEN_SOURCE 700
#include "job.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

int jobs(void) {
    int no_job = 1;
    for (int i = 0; i < MAX_JOBS; i++) {
        //fprintf(stdout, "job status : %d\n", jobs_list[i].status);
        if (jobs_list[i].status != -1) {
            no_job = 0;
            const char* status_str;
            int should_remove = 0;
            switch (jobs_list[i].status) {
                case 0:
                    status_str = "Running";
                    break;
                case 1:
                    status_str = "Stopped";
                    break;
                case 2:
                    status_str = "Done";
                    should_remove = 1;
                    break;
                default:
                    status_str = "Unknown";
            }
            fprintf(stdout, "[%d] PID: %d, Command: %s, Status: %s\n", i + 1, jobs_list[i].pid, jobs_list[i].command, status_str);
            
            if (should_remove) {
                remove_job(jobs_list[i].pid);
            }
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

int add_job(pid_t pid, const char* command) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs_list[i].status == -1) {
            jobs_list[i].pid = pid;
            jobs_list[i].command = strdup(command);
            jobs_list[i].status = 0; // running
            return 0; // success
        }
    }
    return -1; // no space for new job
}

void handle_sigchld(int sig){
    int status;
    pid_t pid;

    // Reap all available children without blocking
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        for (int i = 0; i < MAX_JOBS; i++) {
            if (jobs_list[i].pid == pid) {
                if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    jobs_list[i].status = 2; // Done
                } else if (WIFSTOPPED(status)) {
                    jobs_list[i].status = 1; // Stopped
                } else if (WIFCONTINUED(status)) {
                    jobs_list[i].status = 0; // Running
                }
                break;
            }
        }
    }
}

int fg(char **args) {
    if (!args[1]) {
         fprintf(stderr, "fg: argument required\n");
         return -1;
    }
    int job_id = atoi(args[1]);
    if (job_id < 1 || job_id > MAX_JOBS) {
        fprintf(stderr, "fg: invalid job id\n");
        return -1;
    }
    
    // Check if job exists
    int idx = job_id - 1;
    if (jobs_list[idx].status == -1) {
        fprintf(stderr, "fg: no such job\n");
        return -1;
    }
    
    pid_t pid = jobs_list[idx].pid;
    
    // Block SIGCHLD
    sigset_t mask, prev_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, &prev_mask);
    
    // Give terminal
    tcsetpgrp(STDIN_FILENO, pid);
    
    // Send SIGCONT
    if (kill(-pid, SIGCONT) < 0) {
        // perror("kill"); 
    }
    
    int status;
    waitpid(pid, &status, WUNTRACED);
    
    tcsetpgrp(STDIN_FILENO, getpgrp());
    
    if (WIFSTOPPED(status)) {
        jobs_list[idx].status = 1;
        printf("\n[%d] Stopped %s\n", job_id, jobs_list[idx].command);
    } else if (WIFEXITED(status) || WIFSIGNALED(status)) {
        jobs_list[idx].status = 2; // Done
        remove_job(pid);
    }
    
    sigprocmask(SIG_SETMASK, &prev_mask, NULL);
    return 0;
}

int bg(char **args) {
    if (!args[1]) {
         fprintf(stderr, "bg: argument required\n");
         return -1;
    }
    int job_id = atoi(args[1]);
    if (job_id < 1 || job_id > MAX_JOBS) {
        fprintf(stderr, "bg: invalid job id\n");
        return -1;
    }
    int idx = job_id - 1;
    if (jobs_list[idx].status == -1) {
        fprintf(stderr, "bg: no such job\n");
        return -1;
    }
    
    pid_t pid = jobs_list[idx].pid;
    
    if (kill(-pid, SIGCONT) < 0) {
        perror("kill");
        return -1;
    }
    
    jobs_list[idx].status = 0; // Running
    printf("[%d] %s &\n", job_id, jobs_list[idx].command);
    
    return 0;
}
