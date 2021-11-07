#include "parser1.h"
#include <ctype.h>
#include <string.h>

/* A struct to create a doubly linked list, with each cell containing one simple
 * command */
struct command_list_cell {
    command_simple_t command;       /* A pointer to the simple command */
    struct command_list_cell *next; /* Pointer to the next cell */
    struct command_list_cell *prev; /* Poointer to the previous cell */
};

/* A struct containing the command sequence elements and ways to move among them */
struct command_sequence {
    struct command_list_cell
        *command_list_head; /* A pointer to the head of the simple command list */
    struct command_list_cell
        *current_command; /* A pointer to the current simple command */
    int command_count;    /* The total number of simple commands */
};

struct command_simple {
    char **command_and_args; /* A array of strings containing the command and its
                                arguments */

    int input_redirection; /* Boolean, whether redirection is used */
    char *input_filename;  /* The filename for the input */

    int output_redirection;       /* Boolean, whether redirection is used */
    char *output_filename;        /* The filename for the output */
    enum OUTPUT_TYPE output_type; /* The type of the output */

    int pipeIn;  /* Boolean, whether to use input from the previous pipe */
    int pipeOut; /* Boolean, whether to write the output to the next pipe */
};

char **get_command_and_arguments(command_simple_t simple_command) {
    return simple_command->command_and_args;
}

char *get_input_filename(command_simple_t simple_command) {
    if (simple_command->input_redirection)
        return simple_command->input_filename;
    else
        return NULL;
}

char *get_output_filename(command_simple_t simple_command) {
    if (simple_command->output_redirection)
        return simple_command->output_filename;
    else
        return NULL;
}

enum OUTPUT_TYPE get_output_type(command_simple_t simple_command) {
    return simple_command->output_type;
}

int get_pipe_input(command_simple_t simple_command) {
    return simple_command->pipeIn;
}

int get_pipe_output(command_simple_t simple_command) {
    return simple_command->pipeOut;
}
