#include "cmd.h"
#include <unistd.h>

int cd(char **args)
{
    if (args[1] == NULL){
        args[1] = "~";
    }

    if (strcmp(args[1], "~") == 0){
        args[1] = getenv("HOME");
    }
    return chdir(args[1]);
}