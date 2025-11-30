#include "cmd.h"
#include <string.h>
#include<sys/ptrace.h>
#include<sys/wait.h>
#include<unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#define BUFF_SIZE _SC_PAGE_SIZE

// mydump -p <PID> --start 0x... --end 0x... -o fichier
// mydump -p <PID> --start 0x... --end 0x... -o fichier
int mydump(char **args){
    printf("Starting dump process...\n");
    if (strcmp("-p",args[1]) != 0){
        perror("Error opening file");
        return 1;
    }

    if (strcmp("--start",args[3]) != 0){
        perror("Error opening file");
        return 1;
    }
    if (strcmp("--end",args[5]) != 0){
        perror("Error opening file");
        return 1;
    }
    if (strcmp("-o",args[7]) != 0){
        perror("Error opening file");
        return 1;
    }
    char* pid_str= args[2];
    pid_t pid= atoi(pid_str);
    long start= strtol(args[4], NULL, 16);
    long end= strtol(args[6], NULL, 16);
    char* output_path= args[8];


    char path[200];
    sprintf(path, "/proc/%s/mem", pid_str);

    int mem_fd = open(path, O_RDONLY);
    if (mem_fd == -1) {
        // may need root privileges
        perror("Error opening mem file");
        return 1;
    }

    int out_fd = open(output_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd == -1) {
        perror("Error opening output file");
        close(mem_fd);
        return 1;
    }

    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
        perror("ptrace attach failed");
        close(mem_fd);
        close(out_fd);
        return 1;
    }

    waitpid(pid, NULL, 0);

    if (lseek(mem_fd, start, SEEK_SET) == -1) {
        perror("lseek failed");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        close(mem_fd);
        close(out_fd);
        return 1;
    }

    //BUFF_SIZE defined as system page size (why not)
    char buf[BUFF_SIZE];

    size_t total_to_read = end - start;
    size_t bytes_processed = 0;
    
    while (bytes_processed < total_to_read) {
        // Calculate how much to read in this chunk
        size_t remaining = total_to_read - bytes_processed;
        //last chunk might be smaller than BUFF_SIZE
        size_t chunk_size = (remaining < BUFF_SIZE) ? remaining : BUFF_SIZE;

        ssize_t bytes_read = read(mem_fd, buf, chunk_size);
        
        if (bytes_read <= 0) {
            perror("Read error or End of File encountered prematurely");
            ptrace(PTRACE_DETACH, pid, NULL, NULL);
            close(mem_fd);
            close(out_fd);
            return 1;
        }

        if (write(out_fd, buf, bytes_read) != bytes_read) {
            perror("Write failed");
            ptrace(PTRACE_DETACH, pid, NULL, NULL);
            close(mem_fd);
            close(out_fd);
            return 1;
        }

        bytes_processed += bytes_read;
    }

    ptrace(PTRACE_DETACH, pid, NULL, NULL);
    close(mem_fd);
    close(out_fd);

    printf("Done. Dumped %zu bytes.\n", bytes_processed);
    return 0;
}