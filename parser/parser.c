#include "parser.h"

char** split_to_args(char *line)
{
	int bufsize = TOKEN_BUFSIZE, position = 0;
	char** tokens = malloc(bufsize * sizeof(char*));
	char* token;

	if (!tokens) {
		fprintf(stderr, "shell: allocation error\n");
		exit(EXIT_FAILURE);
  	}

	token = strtok(line, TOKEN_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += TOKEN_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "shell: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, TOKEN_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

// gets line from stdin & ends program when EOF is encountered
char* get_line(void)
{
	char *line = NULL;
	size_t bufsize = 0;

	if (getline(&line, &bufsize, stdin) == -1){
		if (feof(stdin)) {
			free(line);
			printf("\n");
			exit(EXIT_SUCCESS);
		} else  {
			fprintf(stderr, "readline");
			exit(EXIT_FAILURE);
		}
	}
	return line;
}
