#include "cmd.h"

static char* args[] =
{
	"netstat", "-tunap", NULL,
};

int mynetstat(void)
{
	return execvp(args[0], args); // If we return from execvp we have failed.
}
