#include "cmd.h"

int generic_fun(char** args)
{
	return execvp(args[0], args);
}
