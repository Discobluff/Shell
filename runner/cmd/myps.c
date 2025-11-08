#include "cmd.h"

static char* args[] =
{
	"ps", "-eo", "pid,ppid,uid,vsz,rss,cmd", NULL,
};

int myps(void)
{
	return execvp(args[0], args);; // If we return from execvp we have failed.
}
