#include "cmd.h"

const char* builtin_str[] = {
	// without args
	[0] = "myps",
	[1] = "mypstree",
	[2] = "mynetstat",
	[3] = "myarp",
	[4] = "myexe",

	// with args
	[5] = "myenv",
};

const function builtin_func[] = {
	// without args
	[0] = (function) {.type = no_arg, .fun.no_arg = *myps},
	[1] = (function) {.type = no_arg, .fun.no_arg = *mypstree},
	[2] = (function) {.type = no_arg, .fun.no_arg = *mynetstat},
	[3] = (function) {.type = no_arg, .fun.no_arg = *myarp},
	[4] = (function) {.type = no_arg, .fun.no_arg = *myexe},

	// with args
	[5] = (function) {.type = no_arg, .fun.with_arg = *myenv},
};

const function generic = {.type = with_arg, .fun.with_arg = * generic_fun};


inline size_t builtin_number(void)
{
	return sizeof(builtin_str) / sizeof(char *);
}

static void builtin_runner(function fun, char** args)
{
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
	// if we return from fun.fun then we have failed 
	exit(EXIT_FAILURE);
}

// handles the function call to args[0], builtin or generic
void function_handler(char** args)
{
	for (unsigned long j = 0; j < builtin_number(); j++) {
		if (strcmp(args[0], builtin_str[j]) == 0) {
			builtin_runner(builtin_func[j], args);
		}
	}
	builtin_runner(generic, args); // default to generic if not a builtin
}
