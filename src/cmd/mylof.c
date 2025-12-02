#include "cmd.h"
#include <string.h>
#include <stdio.h>     /* for printf and stderr */
#include <errno.h>     /* for errno */
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>   // for DIR, opendir, readdir, and dir
#define BUF 1024

int test_dev_pts(char* path){
    if(path == NULL || strlen(path) < 9){
        return 0;
    }
    if (strncmp(path,"/dev/pts/",9) == 0){
        return 1;
    }
    return 0;
}

int mylof(char **args)
{
    if (args[1] == NULL || strcmp("-p",args[1]) != 0 || args[2] == NULL){
        perror("Usage: mylof -p <PID>");
        return 1;
    }
    

    /* Scanning the target directory */

    char *pid_str = args[2];
    char path[200];
    DIR*           fdir;
    struct dirent* fd;

    sprintf(path, "/proc/%s/fd", pid_str);


    fdir = opendir(path);
    if (fdir == NULL)
    {
        fprintf(stderr, "Error : Failed to open target directory - %s\n", strerror(errno));
        return 1;
    }

    /* Reading object (files, directories ...) from the folder) */
    while ((fd = readdir(fdir))!=NULL)
    {
        if (DT_LNK == fd->d_type)
        {
            char fd_address[BUF];
            char address[BUF];
            printf("File descriptor number %s:", fd->d_name);
            sprintf(fd_address, "/proc/%s/fd/%s", pid_str, fd->d_name);
            if(readlink(fd_address, address, BUF)!=-1){
                printf(" %s -> %s\n", fd_address, address);
                if (test_dev_pts(address)){
                    printf("    Associated with a terminal (dev/pts) number: %s\n",(address+9));
                }
            }
        }
        if (DT_UNKNOWN == fd->d_type)
        {
            char fd_address[BUF];
            char address[BUF];
            printf("File descriptor: %s -> ", fd->d_name);
            sprintf(fd_address, "/proc/%s/fd/%s", pid_str, fd->d_name);
            if(readlink(fd_address, address, BUF)!=-1){
                printf("%s Unkown File Type \n", fd_address);
            }
        }
    }
    
    /* Close the directory */
    closedir(fdir);
    
    return 0;
}

