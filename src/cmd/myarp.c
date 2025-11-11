#include "cmd.h"

static char* args[] =
{
	"arp", "-n", NULL,
};

int myarp(void)
{
	return execvp(args[0], args); // If we return from execvp we have failed.
}
