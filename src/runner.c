#include "runner.h"
#include "cmd/cmd.h"

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

// execute the commands as they come with pipe chaining
static int execute_pipeline(command_t* commands, int cmd_count)
{
	int i;
	pid_t pid;

	if (cmd_count == 1) {
		pid = fork();
		if (pid == 0) {
			// Child process
			function_handler(commands[0].args);
			exit(EXIT_FAILURE);
		} else if (pid < 0) {
			fprintf(stderr, "error forking\n");
			return -1;
		} else {
			// parent process
			int status;
            waitpid(pid, &status, 0);
            return 1;
		}
	}

	int pipefds[2 * (cmd_count - 1)]; // 2 file descriptors per pipe

	for (i = 0; i < cmd_count - 1; i++) {
		if (pipe(pipefds + i * 2) == -1) {
			fprintf(stderr, "pipe");
			return -1;
		}
	}

	for (i = 0; i < cmd_count; i++) {
		pid = fork();

		if (pid == 0) {
			// Child process
			if (i > 0) {
				if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) == -1) {
					fprintf(stderr, "dup2");
					exit(EXIT_FAILURE);
				}
			}

			if (i < cmd_count - 1) {
				if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) == -1) {
					fprintf(stderr, "dup2");
					exit(EXIT_FAILURE);
				}
			}

			for (int j = 0; j < 2 * (cmd_count - 1); j++)
				close(pipefds[j]);

			// handle the function
			function_handler(commands[i].args);
			
		} else if (pid < 0) {
			fprintf(stderr, "fork");
			return -1;
		}
	}

	for (i = 0; i < 2 * (cmd_count - 1); i++)
		close(pipefds[i]);

	for (i = 0; i < cmd_count; i++)
		wait(NULL);

	return 1;
}

// runs one command 
int run(char** line)
{
	if (line[0] == NULL) {
		// An empty command ?
		return 1;
	}

	command_t* commands;
	
	int cmd_count = parse_pipeline(line, &commands);
	int result = execute_pipeline(commands, cmd_count);
	
	free(commands);
	return result;
}
