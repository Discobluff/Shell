#ifndef CMD_H
#define CMD_H

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
	enum {with_arg, no_arg} type;
	union fun {
		int (*no_arg) (void);
		int (*with_arg) (char **);
	} fun;
} function;


/**
 * fork-executed functions that don't take arguments
 */
int myps(void);
int mypstree(void);
int mynetstat(void);
int myarp(void);
int myexe(void);
int myinfo(void);

/**
 * fork-executed functions that take arguments
 */
int myenv(char** args);
int cd(char ** args);
int mymaps(char** args);
int mydump(char ** args);

int mylof(char ** args);

/* ls, grep, etc */
int generic_fun(char** args);

/**
 * list of the names of all builtin functions
 */
extern const char* builtin_str[];

/**
 * list of all builtin functions
 */
extern const function builtin_func[];

/**
 * generic function
 */
extern const function generic;

void function_handler(char** args, bool exit_fork);

#endif
