/*
 * Development: Nikos Boumakis, 4346
 * Email: csd4346@csd.uoc.gr
 */

#include "parser.h"

#ifndef SHELL_H
#define SHELL_H

/* Prints the prompt */
void prompt_string();

void execute(command_sequence_t);
#endif