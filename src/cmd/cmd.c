#include "cmd.h"
#include <errno.h>
#include <stdbool.h>

const char* builtin_str[] = {
	// without args
	[0] = "myps",
	[1] = "mypstree",
	[2] = "mynetstat",
	[3] = "myarp",
	[4] = "myexe",

	// with args
	[5] = "myenv",
	[6] = "cd",
};

const function builtin_func[] = {
	// without args
	[0] = (function) {.type = no_arg, .fun.no_arg = myps},
	[1] = (function) {.type = no_arg, .fun.no_arg = mypstree},
	[2] = (function) {.type = no_arg, .fun.no_arg = mynetstat},
	[3] = (function) {.type = no_arg, .fun.no_arg = myarp},
	[4] = (function) {.type = no_arg, .fun.no_arg = myexe},

	// with args
	[5] = (function) {.type = with_arg, .fun.with_arg = myenv},
	[6] = (function) {.type = with_arg, .fun.with_arg = cd},
};

const function generic = {.type = with_arg, .fun.with_arg = generic_fun};


static inline size_t builtin_number(void)
{
	return sizeof(builtin_str) / sizeof(char *);
}

static void builtin_runner(function fun, char** args, bool exit_fork)
{
	switch (fun.type)
	{
	case no_arg:
		if (fun.fun.no_arg() == -1) {
			fprintf(stderr, "Command failed: %s\n", strerror(errno));
			if (exit_fork) exit(EXIT_FAILURE);
		}
		break;
	case with_arg:
		if (fun.fun.with_arg(args) == -1) {
			fprintf(stderr, "Command failed: %s\n", strerror(errno));
			if (exit_fork) exit(EXIT_FAILURE);
		}
		break;
	}
}

// handles the function call to args[0], builtin or generic
void function_handler(char** args, bool exit_fork)
{
	bool standart_func = false;
	for (unsigned long j = 0; j < builtin_number(); j++) {
		if (strcmp(args[0], builtin_str[j]) == 0) {
			standart_func = true;
			builtin_runner(builtin_func[j], args, exit_fork);
		}
	}
	if (!standart_func){
		builtin_runner(generic, args, exit_fork); // default to generic if not a builtin
	}
	// the generic is the only one that may have this problem...
	if (generic.fun.with_arg(args) == -1) fprintf(stderr, "shell: %s: function does not exist\n", args[0]);
	exit(EXIT_FAILURE);
}
