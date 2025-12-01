#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>

#include <stdio.h>
#include <limits.h>

#define TOKEN_BUFSIZE 64
#define TOKEN_DELIM " \t\r\n\a"
#define TOKEN_SEP ";"

typedef enum {
	COND_NONE,
	COND_AND,
	COND_OR,
} ActionCond;

extern int debug;


typedef struct {
	char** args;		// Arguments de la commande
	char* input_file;	// NULL ou nom du fichier pour <
	char* output_file;	// NULL ou nom du fichier pour >
	bool append;		// 0 pour >, 1 pour >>
	char* error_file;	// NULL ou nom du fichier pour 2>
	bool need_fork;
} command_t;

// functions for command line
// int split_to_args_list(char *line, char ****args_list);
int split_to_args_list(char *line, char ****args_list, ActionCond **actionsCond);
char** split_to_args(char *line);
char* get_line(void);

// parser for runners
int parse_pipeline(char** line, command_t** commands);
void free_commands(command_t* commands, int cmd_count);

#endif
