#include <stdio.h>
#include <stdlib.h>
#include "runner.h"
#include "parser/parser.h"

#define MAX_USER_SIZE 32
#define MAX_NAME_SIZE 32

extern char** environ;

int debug = 0;

int main(int argc, char** argv)
{
	char* input;
	char** args;
	char path[1024];
	int status = 1;

	if (argc > 1 && strcmp(argv[1], "--debug") == 0) {
		debug = 1;
	}

	char user[MAX_USER_SIZE],
		 name[MAX_NAME_SIZE],
		 computer_name[MAX_USER_SIZE + MAX_NAME_SIZE + 2];

	int i = 0;
	int has_name = 0;
	while(environ[i]) {
		if (strncmp(environ[i], "USER", 4) == 0)
			strncpy(user, environ[i]+5, MAX_USER_SIZE);
		else if (strncmp(environ[i], "NAME", 4) == 0) {
			strncpy(name, environ[i]+5, MAX_NAME_SIZE);
			has_name = 1;
		}
		i++;
	}

	if (has_name) {
		snprintf(computer_name, MAX_USER_SIZE + MAX_NAME_SIZE + 1, "%s@%s:", user, name);
	}
	else {
		snprintf(computer_name, MAX_USER_SIZE + 1, "%s:", user);
	}

	do {
		getcwd(path, sizeof(path));
		printf("\033[31m%s\033[m\033[32m%s\033[m> ", computer_name, path); fflush(stdout);
		input = get_line();

		if (!isatty(STDIN_FILENO)) printf("%s", input);
		
		args = split_to_args(input);

		status = run(args);

		free(input); free(args);
	} while(status);

	return 0;
}
