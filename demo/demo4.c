#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <bits/mman-linux.h>

int main(){

    //print pid
    printf("PID: %d\n", getpid());
    printf("Demo4: analysis of the executable area\n");

    //make a exec call to demo_sus1
    execl("./demo/demo_sus1", "./demo/demo_sus1", NULL);

}