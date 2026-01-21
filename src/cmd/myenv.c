#include "cmd.h"
#include <string.h>
#include <errno.h>

int myenv(char **args)
{
	if (args[1] == NULL || strcmp("-p",args[1]) != 0 || args[2] == NULL){
        printf("Usage: myenv -p <PID>\n");
        return EINVAL;
    }

	if (args[1] != NULL && strcmp("-p",args[1]) == 0){
		char *pid = args[2];
		char path[200];
		sprintf(path, "/proc/%s/environ", pid);
		FILE *file = fopen(path, "r");
		if (file == NULL) {
			errno = ESRCH;
			return -1;
		}

		char c;
		while ((c = fgetc(file)) != EOF) {
			if (c == '\0') {
			putchar('\n');
			} else {
			putchar(c);
			}
		}

		fclose(file);
		return 0;
	}
	errno = EINVAL;
	return -1;
}
