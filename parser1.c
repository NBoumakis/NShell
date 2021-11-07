#include "parser1.h"
#include "parserUtilities.h"
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

static void insert_after_simple_command_list(struct command_list_cell **,
                                             command_simple_t);

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

command_simple_t get_next_simple_command(command_sequence_t command_seq) {
    command_seq->current_command = command_seq->current_command->next;

    return command_seq->current_command->command;
}

command_simple_t get_previous_simple_command(command_sequence_t command_seq) {
    command_seq->current_command = command_seq->current_command->prev;

    return command_seq->current_command->command;
}

command_simple_t get_current_simple_command(command_sequence_t command_seq) {
    return command_seq->current_command->command;
}

/* Parses the read command or series of commands */
command_sequence_t parse(char *input, size_t input_size,
                         command_sequence_t command_seq) {
    int i, input_index = 0, arg_count = 0;
    command_simple_t tmp_simple;
    char **cmd_args;
    size_t cmd_start_index, cmd_end_index, arg_end_index;
    int prevPipeOut = 0;

    if (command_seq == NULL) {
        command_seq = malloc(sizeof(command_sequence_t));
    }

    command_seq->command_count = count_simple_commands(input, input_size);

    for (i = 0; i < command_seq->command_count; i++) {
        tmp_simple = malloc(sizeof(command_simple_t));
        tmp_simple->input_redirection = 0;
        tmp_simple->input_filename = NULL;
        tmp_simple->output_redirection = 0;
        tmp_simple->output_filename = NULL;

        cmd_start_index = find_command_beginning(input, cmd_start_index, input_size);
        cmd_end_index = find_command_end(input, cmd_start_index, input_size);
        arg_end_index = find_arg_end(input, cmd_start_index, cmd_end_index);

        tmp_simple->command_and_args =
            extract_cmd_args(input, cmd_start_index, arg_end_index);

        if (check_input_redirection(input, arg_end_index + 1, cmd_end_index)) {
            tmp_simple->input_redirection = 1;
            tmp_simple->input_filename = get_filename_input_redirection(
                input, arg_end_index + 1, cmd_end_index);
        }

        if (check_output_redirection(input, arg_end_index + 1, cmd_end_index)) {
            tmp_simple->output_redirection = 1;
            tmp_simple->output_type =
                get_output_redir_type(input, arg_end_index + 1, cmd_end_index);
            tmp_simple->output_filename = get_filename_output_redirection(
                input, arg_end_index + 1, cmd_end_index);
        }

        if (check_pipeOut(input, cmd_start_index, cmd_end_index)) {
            if (!tmp_simple->output_redirection) {
                tmp_simple->pipeOut = 1;
                prevPipeOut = 1;
            } else {
                prevPipeOut = 0;
            }
        } else {
            prevPipeOut = 0;
        }

        if (prevPipeOut) {
            if (!tmp_simple->input_redirection) {
                tmp_simple->pipeIn = 1;
            }
        }

        insert_after_simple_command_list(&(command_seq->command_list_head),
                                         tmp_simple);
    }

    return command_seq;
}

/* Clears the parser and its fields allowing it to be reused */
void clear_command_sequence(command_sequence_t);

/* Clears and deallocates the parser */
void free_command_sequence(command_sequence_t);
