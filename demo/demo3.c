#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <bits/mman-linux.h>

// /* Some systems expose MAP_ANON instead of MAP_ANONYMOUS; provide a fallback */
// #ifndef MAP_ANONYMOUS
// #ifdef MAP_ANON
// #define MAP_ANONYMOUS MAP_ANON
// #endif
// #endif

//gcc demo/demo_sus1.c -o demo/demo_sus1 -z execstack
int main(int argc, char **argv){
    //print pid and exe path
    printf("PID: %d\n", getpid());
    printf("Demo Suspicious File\n");

    // Create in-memory file with custom name
    // In /proc/[pid]/maps, this appears as /memfd:[v_sus0] (deleted)
    int fd = syscall(SYS_memfd_create, "[v_sus0]", 0);
    ftruncate(fd, 4096);

    //WX region ->suspicious
    void *addr = mmap(NULL, 4096,
                      PROT_WRITE | PROT_EXEC,
                      MAP_SHARED,
                      fd, 0);

    
    //RX region with a name (not [vdso]) ->suspicious
    int fd1 = syscall(SYS_memfd_create, "[v_sus2]", 0);
    ftruncate(fd1, 4096);

    void *addr1 = mmap(NULL, 4096,
                       PROT_READ |PROT_EXEC,
                      MAP_SHARED,
                      fd1, 0);

    //X region with anonymous mapping (empty pathname)->suspicious
    void *addr2 = mmap(NULL, 4096,
                    PROT_EXEC,
                    MAP_PRIVATE | MAP_ANONYMOUS,
                    -1, 0);



    while (1)
    {
        sleep(1);
    }
    
    return 0;
}