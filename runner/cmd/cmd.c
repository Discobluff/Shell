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

size_t builtin_number(void)
{
	return sizeof(builtin_str) / sizeof(char *);
}
