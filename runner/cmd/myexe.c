#include "cmd.h"

static char* args[] =
{
	"echo", "\"TODO\"", NULL,
};

int myexe(void)
{
	execvp(args[0], args);
	return -1; // If we return from execvp we have failed.
}
