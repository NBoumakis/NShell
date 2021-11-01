#include "parser.h"

#ifndef SHELL_H
#define SHELL_H

/* Returns a pointer to a string containing the prompt */
char *prompt_string();

void execute(sh_parser_t);
#endif