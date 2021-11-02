#include "parser.h"

#ifndef SHELL_H
#define SHELL_H

/* Prints the prompt */
void prompt_string();

void execute(sh_parser_t);
#endif