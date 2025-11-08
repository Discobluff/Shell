#include "runner.h"
#include "cmd/cmd.h"

static int launch(function fun, char** args)
{
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0) {
		// Child process
		switch (fun.type)
		{
		case no_arg:
			if (fun.fun.no_arg() == -1) {
				fprintf(stderr, "error in child\n");
			}
			break;
		case with_arg:
			if (fun.fun.with_arg(args) == -1) {
				fprintf(stderr, "error in child\n");
			}
			break;
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		fprintf(stderr, "error forking\n");
	} else {
		// Parent process
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

int run(char **args)
{
	unsigned long i;

	if (args[0] == NULL) {
		// An empty command ?
		return 1;
	}

	for (i = 0; i < builtin_number(); i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return launch(builtin_func[i], args);
		}
	}

	return launch(generic, args);
}
