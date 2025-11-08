#include "runner.h"
#include "cmd/cmd.h"
#include "parser/parser.h"

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
