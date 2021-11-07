#include "parser.h"
#include <stddef.h>

int count_simple_commands(char *, size_t);

int count_arguments(char *, size_t, size_t);

size_t find_command_beginning(char *, size_t, size_t);

size_t find_command_end(char *, size_t, size_t);

size_t find_arg_end(char *, size_t, size_t);

char **extract_cmd_args(int, char *, size_t, size_t);

int check_input_redirection(char *, size_t, size_t);

int check_output_redirection(char *, size_t, size_t);

char *get_filename_input_redirection(char *, size_t, size_t);

char *get_filename_output_redirection(char *, size_t, size_t);

enum OUTPUT_TYPE get_output_redir_type(char *, size_t, size_t);

int check_pipeOut(char *, size_t, size_t);