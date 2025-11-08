#include "cmd.h"

static char* args[] =
{
	"netstat", "-tunap", NULL,
};

int mynetstat(void)
{
	execvp(args[0], args);
	return -1; // If we return from execvp we have failed.
}
