#include "parser.h"

char** split_to_args(char *line)
{
	int bufsize = TOKEN_BUFSIZE, position = 0;
	char** tokens = malloc(bufsize * sizeof(char*));
	char* token;

	if (!tokens) {
		perror("shell: allocation error\n");
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
				perror("shell: allocation error\n");
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
			perror("readline");
			exit(EXIT_FAILURE);
		}
	}
	return line;
}

// returns the number of commands separated by pipes and puts them in commands
static int parse_pipeline(char** line, command_t** commands) {
	int cmd_count = 0,
		start = 0,
		i = 0;

	// number of commands = number of pipes + 1
	while (line[i] != NULL) {
		if (strcmp(line[i], "|") == 0) {
			cmd_count++;
		}
		i++;
	}
	cmd_count++;

	*commands = malloc(sizeof(command_t) * cmd_count);

	int cmd_idx = 0;
	start = 0;
	for (i = 0; line[i] != NULL || start < i; i++) {
		if (line[i] == NULL || strcmp(line[i], "|") == 0) {
			(*commands)[cmd_idx].args = &line[start];
			(*commands)[cmd_idx].argc = i - start;

			if (line[i] != NULL)
				line[i] = NULL;

			start = i + 1;
			cmd_idx++;
		}
	}

	return cmd_count;
}
