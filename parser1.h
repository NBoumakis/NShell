#include <stdlib.h>

#ifndef PARSER_H
#define PARSER_H

typedef struct command_sequence *command_sequence_t;
typedef struct command_simple *command_simple_t;

enum OUTPUT_TYPE { OVERWRITE, APPEND };

/* Parses the read command or series of commands */
command_sequence_t parse(char *, size_t, command_sequence_t);

/* Clears the parser and its fields allowing it to be reused */
void clear_command_sequence(command_sequence_t);

/* Clears and deallocates the parser */
void free_command_sequence(command_sequence_t);

/* Return the next token and move current to be the token forward.
 * Returns NULL if out of bounds. */
command_simple_t get_next_simple_command(command_sequence_t);

/* Return the previous token and move current to be the token backwards.
 * Returns NULL if out of bounds */
command_simple_t get_previous_simple_command(command_sequence_t);

/* Return the current token or NULL if out of bounds */
command_simple_t get_current_simple_command(command_sequence_t);

char **get_command_and_arguments(command_simple_t);
char *get_input_filename(command_simple_t);
char *get_output_filename(command_simple_t);
enum OUTPUT_TYPE get_output_type(command_simple_t);
int get_pipe_input(command_simple_t);
int get_pipe_output(command_simple_t);

#endif