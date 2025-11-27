#include "runner.h"
#include "cmd/cmd.h"
#include "parser/parser.h"

// execute the commands as they come with pipe chaining
static int execute_pipeline(command_t* commands, int cmd_count)
{
	int i;
	pid_t pid;

	// one command, don't handle pipes.
	if (cmd_count == 1) {
		int fd_out, fd_in, fd_err;
		if (commands[0].output_file != NULL){
			int flags;
			if (commands[0].append)
				flags = O_WRONLY | O_CREAT | O_APPEND;
			else
				flags = O_WRONLY | O_CREAT | O_TRUNC;

			fd_out = open(commands[0].output_file, flags, 0644);
		}
		if (commands[0].input_file != NULL){
			fd_in= open(commands[0].input_file, O_RDONLY);
			if (fd_in == -1){
				fprintf(stderr, "could not open file: \"%s\"\n", commands[0].input_file);
				exit(EXIT_FAILURE);
			}
		}
		if (commands[0].error_file != NULL){
			fd_err = open(commands[0].error_file, O_WRONLY | O_CREAT | O_TRUNC);
		}

		if (!commands[0].need_fork){
			pid = 0;
		}
		else{
			pid = fork();
		}
		if (pid == 0) {
			// Child process

			// handle input / output / errors in the fork
			if (commands[0].output_file != NULL){
				if (dup2(fd_out, STDOUT_FILENO) == -1){
					fprintf(stderr, "dup2 stdout failed\n");
					exit(EXIT_FAILURE);
				}
				close(fd_out);
			}
			if (commands[0].input_file != NULL){
				if (dup2(fd_in, STDIN_FILENO) == -1){
					fprintf(stderr, "dup2 stdin failded\n");
					exit(EXIT_FAILURE);
				}
				close(fd_in);
			}
			if (commands[0].error_file != NULL){
				if (dup2(fd_err, STDERR_FILENO) == -1){
					fprintf(stderr, "dup2 stderr failed\n");
					exit(EXIT_FAILURE);
				}
				close(fd_err);
			}
			function_handler(commands[0].args, commands[0].need_fork);
			return 1;

		} else if (pid < 0) {
			fprintf(stderr, "error forking\n");
			return -1;

		} else {
			// Parent process
			int status;
            waitpid(pid, &status, 0);
            return 1;
		}
	}

	int pipefds[2 * (cmd_count - 1)]; // 2 file descriptors per pipe

	// create all the pipes
	for (i = 0; i < cmd_count - 1; i++) {
		if (pipe(pipefds + i * 2) == -1) {
			fprintf(stderr, "pipe");
			return -1;
		}
	}

	for (i = 0; i < cmd_count; i++) {
		if (!commands[i].need_fork){
			pid = 0;
		}
		else{
			pid = fork();
		}
		if (pid == 0) {
			// Child process

			// only setup stdin for first command
			if (i == 0) {
				// First command: stdin from file if specified
				if (commands[i].input_file != NULL) {
					int fd_in = open(commands[i].input_file, O_RDONLY);
					if (fd_in == -1) {
						fprintf(stderr, "could not open file: \"%s\"\n", commands[i].input_file);
						exit(EXIT_FAILURE);
					}
					if (dup2(fd_in, STDIN_FILENO) == -1) {
						fprintf(stderr, "dup2 stdin failed\n");
						exit(EXIT_FAILURE);
					}
					close(fd_in);
				}
			} else {
				if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) == -1) {
					fprintf(stderr, "dup2 pipe stdin failed\n");
					exit(EXIT_FAILURE);
				}
			}
			
			// setup stdout only for last command
			if (i == cmd_count - 1) {
				if (commands[i].output_file != NULL) {
					int flags;
					if (commands[i].append)
						flags = O_WRONLY | O_CREAT | O_APPEND;
					else
						flags = O_WRONLY | O_CREAT | O_TRUNC;
					
					int fd_out = open(commands[i].output_file, flags, 0644);
					if (fd_out == -1) {
						fprintf(stderr, "could not open output file: \"%s\"\n", commands[i].output_file);
						exit(EXIT_FAILURE);
					}
					if (dup2(fd_out, STDOUT_FILENO) == -1) {
						fprintf(stderr, "dup2 stdout failed\n");
						exit(EXIT_FAILURE);
					}
					close(fd_out);
				}
			} else {
				if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) == -1) {
					fprintf(stderr, "dup2 pipe stdout failed\n");
					exit(EXIT_FAILURE);
				}
			}
			
			// always setup stderr
			if (commands[i].error_file != NULL) {
				int fd_err = open(commands[i].error_file, O_WRONLY | O_CREAT | O_TRUNC, 0644); // bug: created with 001 permissions...
				if (fd_err == -1) {
					fprintf(stderr, "could not open error file: \"%s\"\n", commands[i].error_file);
					exit(EXIT_FAILURE);
				}
				if (dup2(fd_err, STDERR_FILENO) == -1) {
					fprintf(stderr, "dup2 stderr failed\n");
					exit(EXIT_FAILURE);
				}
				close(fd_err);
			}

			for (int j = 0; j < 2 * (cmd_count - 1); j++)
				close(pipefds[j]);

			// handle the function
			function_handler(commands[i].args, commands[i].need_fork);
			
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

	if (strcmp("fin", line[0]) == 0 && line[1] == NULL){
		return 0;
	}

	command_t* commands;
	
	int cmd_count = parse_pipeline(line, &commands);
	
	// prints the chained commands if debug is set to 1
	for (int i = 0; i < cmd_count*debug; i++){
		printf("Command %d: name %s\ninput: %s\noutput: %s\nerror: %s\n",
			i + 1,
			commands[i].args[0],
			commands[i].input_file,
			commands[i].output_file,
			commands[i].error_file
		);
	}

	int result = execute_pipeline(commands, cmd_count);

	free_commands(commands, cmd_count);
	return result;
}
