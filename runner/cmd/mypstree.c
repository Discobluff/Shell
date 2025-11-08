#include "cmd.h"

static char* args[] =
{
	"pstree", "-p", NULL,
};

int mypstree(void)
{
	execvp(args[0], args);
	return -1; // If we return from execvp we have failed.
}
