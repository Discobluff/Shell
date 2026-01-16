#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

int main(){
    while (true){
        malloc(1024*1024);
        sleep(2);
    }
    return 0;
}
