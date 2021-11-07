#include "parserUtilities.h"
#include <ctype.h>
#include <string.h>

static int cmd_valid_char(char c) { return isalnum(c) || c == '<' || c == '>'; }
static int is_separator(char c) { return (c == ';' || c == '|'); }

int count_arguments(char *input, size_t arg_start, size_t arg_end) {
    size_t i;
    int arg_count = 0;
    int in_arg = 0;
    char c;

    for (i = arg_start; i <= arg_end; i++) {
        c = input[i];

        if (!in_arg) {
            if (cmd_valid_char(c)) {
                in_arg = 1;
                arg_count++;
            }
        } else {
            if (isspace(c))
                in_arg = 0;
        }
    }

    return arg_count;
}

int count_simple_commands(char *input, size_t input_size) {
    size_t i;
    int simple_commands = 0;
    int in_command = 0;
    char c;

    i = 0;
    while (i < input_size) {
        c = input[i];

        if (!in_command) {
            if (cmd_valid_char(c)) {
                in_command = 1;
                simple_commands++;
            }
        } else {
            if (is_separator(c))
                in_command = 0;
        }

        i++;
    }

    return simple_commands;
}

size_t find_command_beginning(char *input, size_t from, size_t to) {
    for (; from <= to; from++) {
        if (cmd_valid_char(input[from])) {
            return from;
        }
    }

    return -1;
}

size_t find_command_end(char *input, size_t from, size_t to) {
    for (; from <= to; from++) {
        if (is_separator(input[from])) {
            break;
        }
    }

    for (to = from; to > 0; to--) {
        if (cmd_valid_char(input[to])) {
            return to;
        }
    }

    return -1;
}

size_t find_arg_end(char *input, size_t from, size_t to) {
    for (; from <= to; from++) {
        if (is_separator(input[from]) || input[from] == '<' || input[from] == '>') {
            break;
        }
    }

    for (to = from - 1; to > 0; to--) {
        /* I already know that there are no '<' or '>' before from */
        if (cmd_valid_char(input[to])) {
            return to;
        }
    }

    return -1;
}

char **extract_cmd_args(char *input, size_t arg_start, size_t arg_end) {
    size_t arg_count = count_arguments(input, arg_start, arg_end);
    char **cmd_args = malloc(arg_count * (sizeof(char *)));
    size_t arg_size, i;

    for (i = 0; i < arg_count; i++) {
        arg_size = 0;

        while (isspace(input[arg_start])) {
            arg_start++;
        }

        while (!isspace(input[arg_start + arg_size])) {
            arg_size++;
        }

        cmd_args[i] = malloc((arg_size + 1) * sizeof(char));
        strncpy(cmd_args[i], &(input[arg_start]), arg_size);
        cmd_args[i][arg_size] = '\0';

        arg_start += arg_size + 1;
    }

    return cmd_args;
}

int check_input_redirection(char *input, size_t from, size_t to) {
    size_t i;

    for (i = from; i <= to; i++) {
        if (input[i] == '<')
            return 1;
    }

    return 0;
}

int check_output_redirection(char *input, size_t from, size_t to) {
    size_t i;

    for (i = from; i <= to; i++) {
        if (input[i] == '>')
            return 1;
    }

    return 0;
}

char *get_filename_input_redirection(char *input, size_t from, size_t to) {
    size_t i, filename_size = 0, filename_start;
    char *filename;

    for (i = from; i <= to; i++) {
        if (input[i] == '<')
            break;
    }

    i++;
    while (i <= to && isspace(input[i])) {
        i++;
    }

    if (i > to) {
        return NULL;
    }

    filename_start = i;
    while (i <= to && !isspace(input[i])) {
        filename_size++;
        i++;
    }

    filename = malloc((filename_size + 1) * sizeof(char));
    strncpy(filename, &(input[filename_start]), filename_size);
    filename[filename_size] = '\0';

    return filename;
}

char *get_filename_output_redirection(char *input, size_t from, size_t to) {
    size_t i, filename_size = 0, filename_start;
    char *filename;

    for (i = from; i < to; i++) {
        if (input[i] == '>') {
            break;
        }
    }

    i++;

    if (input[i] == '>') {
        i++;
    }

    while (i <= to && isspace(input[i])) {
        i++;
    }

    if (i > to) {
        return NULL;
    }

    filename_start = i;
    while (i <= to && !isspace(input[i])) {
        filename_size++;
        i++;
    }

    filename = malloc((filename_size + 1) * sizeof(char));
    strncpy(filename, &(input[filename_start]), filename_size);
    filename[filename_size] = '\0';

    return filename;
}

enum OUTPUT_TYPE get_output_redir_type(char *input, size_t from, size_t to) {
    size_t i;

    for (i = from; i < to; i++) {
        if (input[i] == '>') {
            break;
        }
    }

    i++;

    if (input[i] == '>') {
        return APPEND;
    }

    return OVERWRITE;
}

int check_pipeOut(char *input, size_t from, size_t to) {
    for (; from <= to; from++) {
        if (input[from] == '|')
            return 1;
    }

    return 0;
}