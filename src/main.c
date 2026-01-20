#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "runner.h"
#include "parser/parser.h"
#include <errno.h>
#include "cmd/cmd.h"
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_USER_SIZE 32
#define MAX_NAME_SIZE 32



job_t jobs_list[MAX_JOBS];
int debug = 0;

void free_args(char **args){
	int i = 0;
	while (args[i]){
		free(args[i]);
		i++;
	}
	free(args);
}

int main(int argc, char** argv)
{
	//JOB CONTROL INIT
	for (int i = 0; i < MAX_JOBS; i++) {
	jobs_list[i].pid = -1;
	jobs_list[i].command = NULL;
	jobs_list[i].status = -1; // no job running
	}
	//handler for SIGCHLD
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
	sigaction(SIGCHLD, &sa, NULL);

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

	char* input = NULL;
	char path[1024];
	char showPath[1024];
	char *home = getenv("HOME");
	int status = 1;

	if (argc > 1 && strcmp(argv[1], "--debug") == 0) {
		debug = 1;
	}

	char* user = getenv("USER"),
		 *name = getenv("NAME"),
		 *computer_name  = malloc(sizeof(char)*(MAX_USER_SIZE + MAX_NAME_SIZE + 1));


	if (name) {
		sprintf(computer_name, "%s@%s:", user, name);
	}
	else {
		snprintf(computer_name, MAX_USER_SIZE + 1, "%s:", user);
	}
	do {
		getcwd(path, sizeof(path));
		if (strncmp(path, home, strlen(home)) == 0){
			snprintf(showPath, sizeof(showPath), "~%s", path+strlen(home));
		}
		else{
			strcpy(showPath, path);
		}
		char message[2048];
		if (errno){
			snprintf(message, sizeof(message), "\033[31m➜ %s\033[m\033[34m%s\033[m > ", computer_name, showPath);
		}
		else{
			snprintf(message, sizeof(message), "\033[32m➜ \033[m\033[31m%s\033[m\033[34m%s\033[m > ", computer_name, showPath);
		}
		if (!isatty(STDIN_FILENO)) {
			printf("%s",message);
			fflush(stdout);
			input = readline(NULL);
		}
		else{
			input = readline(message);
		}
		if (input == NULL){
			fflush(stdout);
			break;
		}
		fflush(stdout);
		add_history(input);
		
		char ***args_list;
		ActionCond *actionsCond;
		int count_cmd = split_to_args_list(input, &args_list, &actionsCond);
		for (int i=0;i<count_cmd;){
			if (args_list[i][0] == NULL){
				i++;
				continue;
			}
			if (args_list[i][0][0] == '\0'){
				free_args(args_list[i]);
				i++;
				continue;
			}
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
		free(input); free(args_list); input = NULL;
	} while(status);

	free(computer_name);
	return 0;
}
