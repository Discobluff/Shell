#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<stdbool.h>
#include <unistd.h>
//gcc demo/demo_sus1.c -o demo/demo_sus1 -z execstack
int main(int argc, char **argv){
    //print pid and exe path
    printf("PID: %d\n", getpid());
    printf("Demo Suspicious File\n");

    while (1)
    {
        sleep(1);
    }
    
    return 0;
}