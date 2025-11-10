#include <stdio.h>
#include <stdlib.h>
#include "runner.h"
#include "parser/parser.h"

#define MAX_NAME_SIZE 32

int debug = 0;

int main(int argc, char** argv)
{
	char* input;
	char** args;
	int status = 1;

	if (argc > 1 && strcmp(argv[1], "--debug") == 0) {
		debug = 1;
	}

	char computer_name[MAX_NAME_SIZE];
	int fd = open("/etc/hostname", O_RDONLY);
	int size = read(fd, computer_name, MAX_ARG_SIZE);

	if (size > 0) computer_name[size - 1] = ' ';

	do { 
		printf("\033[31m%s\033[m> ", computer_name); fflush(stdout);

		input = get_line();
		args = split_to_args(input);

		status = run(args);

		free(input); free(args);
	} while(status);

	return 0;
}
