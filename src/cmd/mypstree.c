#include "cmd.h"

static char* args[] =
{
	"pstree", "-p", NULL,
};

int mypstree(void)
{
	return execvp(args[0], args);; // If we return from execvp we have failed.
}
