#include "cmd.h"
#include <string.h>

int myenv(char **args)
{
	if (strcmp("-p",args[1]) == 0){
		char *pid = args[2];
		char path[200];
		sprintf(path, "/proc/%s/environ", pid);
		FILE *file = fopen(path, "r");
		if (file == NULL) {
			perror("Error opening file");
			return 1;
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
	return 1;
}
