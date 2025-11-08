#ifndef CMD_H
#define CMD_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

/**
 * fork-executed functions that take arguments
 */
int myenv(char** args);

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


size_t builtin_number(void);

#endif
