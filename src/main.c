#include <stdio.h>
#include <stdlib.h>
#include "runner.h"
#include "parser/parser.h"
#include <errno.h>
#include "cmd/cmd.h"

#define MAX_USER_SIZE 32
#define MAX_NAME_SIZE 32

extern char** environ;


job_t jobs_list[MAX_JOBS];
int debug = 0;

void free_args(char **args){
	int i = 0;
	while (args[i]){
		// printf("%s\n",args[i]);
		free(args[i]);
		i++;
	}
	free(args);
}

int main(int argc, char** argv)
{
	for (int i = 0; i < MAX_JOBS; i++) {
	jobs_list[i].pid = -1;
	jobs_list[i].command = NULL;
	jobs_list[i].status = -1; // no job running
	}

	char* input;
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
		printf("\033[31m%s\033[m\033[32m%s\033[m > ", computer_name, path); fflush(stdout);
		input = get_line();

		if (!isatty(STDIN_FILENO)) {printf("%s\n", input); fflush(stdout);}
		
		char ***args_list;
		ActionCond *actionsCond;
		int count_cmd = split_to_args_list(input, &args_list, &actionsCond);
		for (int i=0;i<count_cmd;){
			errno = 0;
			status = run(args_list[i]);
			free_args(args_list[i]);
			int j = i+1;
			ActionCond target;
			if (errno){
				target = COND_AND;
			}
			else{
				target = COND_OR;
			}
			while (j < count_cmd && actionsCond[j-1] == target){
				free_args(args_list[j]);
				j++;
			}
			i = j;
		}

		if (actionsCond) free(actionsCond);
		free(input); free(args_list);
	} while(status);

	return 0;
}
