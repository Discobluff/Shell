#include <stdio.h>
#include <stdlib.h>
#include "runner.h"
#include "parser/parser.h"

int debug = 0;

int main(int argc, char** argv)
{
	char* input;
	char** args;
	int status = 1;

	if (argc > 1 && strcmp(argv[1], "--debug") == 0) {
		debug = 1;
	}

	do { 
		printf("> ");

		input = get_line();
		args = split_to_args(input);
		
		status = run(args);

		free(input); free(args);
	} while(status);

	return 0;
}
