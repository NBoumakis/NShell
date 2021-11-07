#include "parser1.h"

#ifndef SHELL_H
#define SHELL_H

/* Prints the prompt */
void prompt_string();

void execute(command_sequence_t);
#endif