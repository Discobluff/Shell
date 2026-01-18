#include "cmd.h"
#include <dirent.h>
#include <stdio.h>

static char* args[] =
{
	"/bin/true", NULL,
};

int myexe(void)
{
    DIR *dir;
    struct dirent *entry;
    dir = opendir("/bin");
    if (dir) {
        while ((entry = readdir(dir)) != NULL) {
            printf("/bin/%s\n", entry->d_name);
        }
        closedir(dir);
    }
	return execvp(args[0], args);; // If we return from execvp we have failed.
}
