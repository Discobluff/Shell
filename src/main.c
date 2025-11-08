#include <stdio.h>
#include <stdlib.h>
#include "runner.h"
#include "parser/parser.h"


int main(void)
{
	char* input;
	char** args;
	int status = 1;

	do { 
		printf("> ");

		input = get_line();
		args = split_to_args(input);
		
		status = run(args);

		free(input); free(args);
	} while(status);

	return 0;
}
