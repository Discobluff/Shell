#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <stdio.h>
#include <limits.h>

#define TOKEN_BUFSIZE 64
#define TOKEN_DELIM " \t\r\n\a"

char** split_to_args(char* input);
char* get_line(void);

#endif
