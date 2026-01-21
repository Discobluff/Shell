#include "cmd.h"
#include <string.h>
#include<sys/ptrace.h>
#include<sys/wait.h>
#include<unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>     /* for errno */
#define BUFF_SIZE _SC_PAGE_SIZE

// mydump -p <PID> --start 0x... --end 0x... -o fichier
// mydump -p <PID> --start 0x... --end 0x... -o fichier
int mydump(char **args){
    if (args[1] ==NULL || strcmp("--help",args[1]) == 0 || strcmp("-h",args[1]) == 0){
        printf("Usage: mydump -p <PID> --start 0x... --end 0x... -o <output_file>\n");
        return EINVAL;
    }

    if (args[1] ==NULL || strcmp("-p",args[1]) != 0){
        printf("Usage: mydump -p <PID> --start 0x... --end 0x... -o <output_file>\n");
        return EINVAL;
    }

    if (args[4]==NULL)
    {
        printf("Usage: mydump -p <PID> --start 0x... --end 0x... -o <output_file>\n");
        return EINVAL;
    }
    
    if (args[3] == NULL || strcmp("--start",args[3]) != 0){
        printf("Usage: mydump -p <PID> --start 0x... --end 0x... -o <output_file>\n");
        return EINVAL;
    }
    if (args[5] == NULL || strcmp("--end",args[5]) != 0){
        printf("Usage: mydump -p <PID> --start 0x... --end 0x... -o <output_file>\n");
        return EINVAL;
    }
    if (args[7] == NULL ||  strcmp("-o",args[7]) != 0){
        printf("Usage: mydump -p <PID> --start 0x... --end 0x... -o <output_file>\n");
        return EINVAL;
    }
    printf("Starting dump process...\n");

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
        printf("Needed privileges to open memory file");
        return EPERM;
    }

    int out_fd = open(output_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd == -1) {
        printf("Error opening output file");
        close(mem_fd);
        return EBADF;
    }

    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
        printf("ptrace attach failed");
        close(mem_fd);
        close(out_fd);
        return EINTR;
    }

    waitpid(pid, NULL, 0);

    if (lseek(mem_fd, start, SEEK_SET) == -1) {
        printf("lseek failed");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        close(mem_fd);
        close(out_fd);
        return EIO;
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
            printf("Read error or End of File encountered prematurely");
            ptrace(PTRACE_DETACH, pid, NULL, NULL);
            close(mem_fd);
            close(out_fd);
            return EIO;
        }

        if (write(out_fd, buf, bytes_read) != bytes_read) {
            printf("Write failed");
            ptrace(PTRACE_DETACH, pid, NULL, NULL);
            close(mem_fd);
            close(out_fd);
            return EIO;
        }

        bytes_processed += bytes_read;
    }

    ptrace(PTRACE_DETACH, pid, NULL, NULL);
    close(mem_fd);
    close(out_fd);

    printf("Done. Dumped %zu bytes.\n", bytes_processed);
    return 0;
}