#include "cmd.h"

static char* args[] =
{
	"arm", "-n", NULL,
};

int myarp(void)
{
	return execvp(args[0], args); // If we return from execvp we have failed.
}
