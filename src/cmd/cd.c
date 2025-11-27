#include "cmd.h"
#include <unistd.h>

int cd(char **args)
{
    return chdir(args[1]);
}