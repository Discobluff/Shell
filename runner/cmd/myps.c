#include "cmd.h"

static char* args[] =
{
	"ps", "-eo", "pid,ppid,uid,vsz,rss,cmd", NULL,
};

int myps(void)
{
	execvp(args[0], args);
	return -1; // If we return from execvp we have failed.
}
