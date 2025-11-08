#include "cmd.h"

static char* args[] =
{
	"arm", "-n", NULL,
};

int myarp(void)
{
	execvp(args[0], args);
	return -1; // If we return from execvp we have failed.
}
