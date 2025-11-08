#include "cmd.h"

static char* args[] =
{
	"echo", "\"TODO\"", NULL,
};

int myexe(void)
{
	return execvp(args[0], args);; // If we return from execvp we have failed.
}
