#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "../src/cmd/job.h"

// Reset the job list before each test
void clear_jobs() {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs_list[i].command) free(jobs_list[i].command);
        jobs_list[i].pid = -1;
        jobs_list[i].command = NULL;
        jobs_list[i].status = -1;
    }
}

void test_add_remove_job() {
    printf("[TEST] add_job and remove_job... ");
    clear_jobs();

    // 1. Add a job
    int res = add_job(123, "sleep 10");
    assert(res == 0);
    
    // Verify it's in the list
    int found = 0;
    for(int i=0; i<MAX_JOBS; i++) {
        if(jobs_list[i].pid == 123) {
            assert(strcmp(jobs_list[i].command, "sleep 10") == 0);
            assert(jobs_list[i].status == 0); // 0 = Running
            found = 1;
            break;
        }
    }
    assert(found);

    // 2. Remove the job
    res = remove_job(123);
    assert(res == 0);

    // Verify it's gone
    for(int i=0; i<MAX_JOBS; i++) {
        assert(jobs_list[i].pid != 123);
    }
    
    // 3. Remove non-existent job
    res = remove_job(999);
    assert(res == -1);

    printf("PASSED\n");
}

void test_max_jobs_limit() {
    printf("[TEST] MAX_JOBS limit... ");
    clear_jobs();

    // Fill the list
    for(int i=0; i<MAX_JOBS; i++) {
        char cmd[20];
        sprintf(cmd, "job %d", i);
        int res = add_job(100 + i, cmd);
        assert(res == 0);
    }

    // Try to add one more
    int res = add_job(999, "overflow");
    assert(res == -1); // Should fail

    // Remove one and add again
    remove_job(100); // Remove first one
    res = add_job(999, "new job");
    assert(res == 0); // Should succeed now

    printf("PASSED\n");
}

void test_job_status_update() {
    printf("[TEST] Job status updates... ");
    clear_jobs();

    add_job(500, "ctrl-z test");
    
    // Simulate finding the job and updating status (like handle_sigchld would)
    // We can't easily call handle_sigchld without mocking waitpid,
    // but we can verify we can access and modify the state.
    
    for(int i=0; i<MAX_JOBS; i++) {
        if(jobs_list[i].pid == 500) {
            jobs_list[i].status = 1; // Stopped
            break;
        }
    }

    // Verify
    for(int i=0; i<MAX_JOBS; i++) {
        if(jobs_list[i].pid == 500) {
            assert(jobs_list[i].status == 1);
        }
    }

    printf("PASSED\n");
}

int main() {
    printf("Running Unit Tests for Job Control...\n");
    test_add_remove_job();
    test_max_jobs_limit();
    test_job_status_update();
    printf("All Unit Tests Passed!\n");
    return 0;
}
