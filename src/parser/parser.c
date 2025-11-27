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

static void init_command(command_t* cmd)
{
	cmd->args = NULL;
	cmd->input_file = NULL;
	cmd->output_file = NULL;
	cmd->append = false;
	cmd->error_file = NULL;
	cmd->need_fork = true;
}

// returns the number of commands separated by pipes and puts them in commands
int parse_pipeline(char** line, command_t** commands)
{
	int cmd_count = 0;
	int i = 0;

	// Compter le nombre de commandes (nombre de pipes + 1)
	while (line[i] != NULL) {
		if (strcmp(line[i], "|") == 0) {
			cmd_count++;
		}
		i++;
	}
	cmd_count++;

	*commands = malloc(sizeof(command_t) * cmd_count);
	if (!*commands) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	int cmd_idx = 0;
	int start = 0;

	for (i = 0; line[i] != NULL || (i > start && cmd_idx < cmd_count); ) {
		init_command(&(*commands)[cmd_idx]);
		
		int end = start;
		while (line[end] != NULL && strcmp(line[end], "|") != 0) {
			end++;
		}
		
		int arg_count = 0;
		int j = start;
		
		while (j < end) {
			if (strcmp(line[j], "<") == 0 || 
				strcmp(line[j], ">") == 0 || 
				strcmp(line[j], ">>") == 0 ||
				strcmp(line[j], "2>") == 0) {
				j += 2;
			} else {
				arg_count++;
				j++;
			}
		}
		
		char** args = malloc(sizeof(char*) * (arg_count + 1));
		if (!args) {
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		
		int arg_idx = 0;
		j = start;
		
		while (j < end) {
			if (strcmp(line[j], "cd") == 0){
				(*commands)[cmd_idx].need_fork = false;
			}
			if (strcmp(line[j], "<") == 0) {
				if (j + 1 < end && line[j + 1] != NULL) {
					(*commands)[cmd_idx].input_file = line[j + 1];
					j += 2;
				} else {
					fprintf(stderr, "Error: missing filename after '<'\n");
					j++;
				}
			} 
			else if (strcmp(line[j], ">") == 0) {
				if (j + 1 < end && line[j + 1] != NULL) {
					(*commands)[cmd_idx].output_file = line[j + 1];
					(*commands)[cmd_idx].append = false;
					j += 2;
				} else {
					fprintf(stderr, "Error: missing filename after '>'\n");
					j++;
				}
			}
			else if (strcmp(line[j], ">>") == 0) {
				if (j + 1 < end && line[j + 1] != NULL) {
					(*commands)[cmd_idx].output_file = line[j + 1];
					(*commands)[cmd_idx].append = true;
					j += 2;
				} else {
					fprintf(stderr, "Error: missing filename after '>>'\n");
					j++;
				}
			}
			else if (strcmp(line[j], "2>") == 0) {
				if (j + 1 < end && line[j + 1] != NULL) {
					(*commands)[cmd_idx].error_file = line[j + 1];
					j += 2;
				} else {
					fprintf(stderr, "Error: missing filename after '2>'\n");
					j++;
				}
			}
			else {
				args[arg_idx++] = line[j];
				j++;
			}
		}
		
		args[arg_idx] = NULL;
		(*commands)[cmd_idx].args = args;
		
		if (line[end] != NULL && strcmp(line[end], "|") == 0) {
			start = end + 1;
			i = start;
		} else {
			i = end;
		}
		
		cmd_idx++;
	}

	return cmd_count;
}

// Fonction pour libérer la mémoire des commandes
void free_commands(command_t* commands, int cmd_count)
{
	for (int i = 0; i < cmd_count; i++) {
		if (commands[i].args != NULL) {
			free(commands[i].args);
		}
	}
	free(commands);
}
