#include "parser.h"
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
        *command_list_tail; /* A pointer to the tail of the simple command list */
    struct command_list_cell
        *current_command; /* A pointer to the current simple command */
    int command_count;    /* The total number of simple commands */
};

struct command_simple {
    char **command_and_args; /* A array of strings containing the command and its
                                arguments */
    int command_args_count;

    int input_redirection; /* Boolean, whether redirection is used */
    char *input_filename;  /* The filename for the input */

    int output_redirection;       /* Boolean, whether redirection is used */
    char *output_filename;        /* The filename for the output */
    enum OUTPUT_TYPE output_type; /* The type of the output */

    int pipeIn;  /* Boolean, whether to use input from the previous pipe */
    int pipeOut; /* Boolean, whether to write the output to the next pipe */
};

static void insert_after_simple_command_list(struct command_list_cell **head,
                                             struct command_list_cell **tail,
                                             command_simple_t simple_command) {
    struct command_list_cell *new_cell = malloc(sizeof(struct command_list_cell));

    new_cell->command = simple_command;

    if (*head == NULL) {
        new_cell->next = NULL;
        new_cell->prev = NULL;

        *head = new_cell;
        *tail = new_cell;

        return;
    }

    new_cell->next = (*tail)->next;
    new_cell->prev = *tail;
    (*tail)->next = new_cell;
    *tail = new_cell;

    return;
}

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

    if (command_seq->current_command)
        return command_seq->current_command->command;
    else
        return NULL;
}

command_simple_t get_previous_simple_command(command_sequence_t command_seq) {
    command_seq->current_command = command_seq->current_command->prev;

    if (command_seq->current_command)
        return command_seq->current_command->command;
    else
        return NULL;
}

command_simple_t get_current_simple_command(command_sequence_t command_seq) {
    if (command_seq->current_command)
        return command_seq->current_command->command;
    else
        return NULL;
}

/* Parses the read command or series of commands */
command_sequence_t parse(char *input, size_t input_size,
                         command_sequence_t command_seq) {
    int i;
    command_simple_t tmp_simple;
    size_t cmd_start_index = 0, cmd_end_index, arg_end_index, separator_index;
    int prevPipeOut = 0;

    if (command_seq == NULL) {
        command_seq = malloc(sizeof(struct command_sequence));
    }

    command_seq->command_list_head = NULL;
    command_seq->command_list_tail = NULL;
    command_seq->command_count = count_simple_commands(input, input_size);

    for (i = 0; i < command_seq->command_count; i++) {
        tmp_simple = malloc(sizeof(struct command_simple));
        tmp_simple->input_redirection = 0;
        tmp_simple->input_filename = NULL;
        tmp_simple->output_redirection = 0;
        tmp_simple->output_filename = NULL;
        tmp_simple->pipeIn = 0;
        tmp_simple->pipeOut = 0;

        cmd_start_index =
            find_command_beginning(input, cmd_start_index, input_size - 1);
        cmd_end_index = find_command_end(input, cmd_start_index, input_size - 1);
        arg_end_index = find_arg_end(input, cmd_start_index, cmd_end_index);
        separator_index = find_separator(input, cmd_start_index, input_size - 1);

        tmp_simple->command_args_count =
            count_arguments(input, cmd_start_index, arg_end_index);

        tmp_simple->command_and_args = extract_cmd_args(
            tmp_simple->command_args_count, input, cmd_start_index, arg_end_index);

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

        if (prevPipeOut) {
            tmp_simple->pipeIn = 1;
        }

        if (check_pipeOut(input, cmd_start_index, separator_index)) {
            tmp_simple->pipeOut = 1;
            prevPipeOut = 1;
        } else {
            prevPipeOut = 0;
        }

        insert_after_simple_command_list(&(command_seq->command_list_head),
                                         &(command_seq->command_list_tail),
                                         tmp_simple);

        cmd_start_index = cmd_end_index + 1;
    }

    command_seq->current_command = command_seq->command_list_head;

    return command_seq;
}

void clear_command_sequence(command_sequence_t command_seq) {
    struct command_list_cell *tmp = command_seq->command_list_head, *next;
    int i;

    while (tmp != NULL) {
        next = tmp->next;

        for (i = 0; i < tmp->command->command_args_count; i++) {
            free(tmp->command->command_and_args[i]);
        }
        free(tmp->command->command_and_args);

        free(tmp->command->input_filename);
        free(tmp->command->output_filename);

        free(tmp->command);
        free(tmp);

        tmp = next;
    }

    command_seq->current_command = NULL;
    command_seq->command_list_head = NULL;
    command_seq->command_count = 0;
}

void free_command_sequence(command_sequence_t command_seq) {
    clear_command_sequence(command_seq);
    free(command_seq);
}
