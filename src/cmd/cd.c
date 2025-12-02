#include "cmd.h"
#include <unistd.h>

int cd(char **args)
{
    char home[1024];
    strcpy(home, getenv("HOME"));
    if (args[1] == NULL){
        return chdir(home);
    }

    if (args[1][0] == '~'){
        args[1] = strcat(home, args[1]+1);
    }
    return chdir(args[1]);
}