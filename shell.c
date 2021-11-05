#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define DIR_MAX_LENGTH 1024 * 8
#define SEP_NUMBER 5

void prompt_string() {
    char *user = getlogin();
    char dir[DIR_MAX_LENGTH];

    getcwd(dir, DIR_MAX_LENGTH);

    printf("%s%s%s", user, "@cs345sh/", dir);
}

static int is_separator(char *input) {
    char *separators[SEP_NUMBER] = {"|", ";", "<", ">", ">>"};
    int i;

    for (i = 0; i < SEP_NUMBER; i++) {
        if (strcmp(input, separators[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

/* Reconstructs a command using the tokens of the parser starting from the one set as
 * the current token to the one before the first separator. It mallocs a char** array
 * used to hold the command tokens, in the order they were given. This array must be
 * freed by the user. After execution, the current token of the parser will be the
 * the last non-separator token before the first separator after the token in which
 * the parser was given in this function */
static char **reconstruct_command(sh_parser_t parser) {
    char **command;
    char *token;
    int arg_count = 0, i;

    token = current_token(parser);

    while (token && !is_separator(token)) {
        arg_count += 1;
        token = next_token(parser);
    }

    command = malloc(arg_count * sizeof(char *));

    for (i = 0; i < arg_count; i++) {
        previous_token(parser);
    }

    command[0] = current_token(parser);

    for (i = 1; i < arg_count; i++) {
        command[i] = next_token(parser);
    }

    previous_token(parser);

    return command;
}

void execute(sh_parser_t parser) {
    char *token = current_token(parser);
    char **command;
    pid_t cpid;

    /* Skip leftover ';' at the beginning of the command */
    while (strcmp(";", token) == 0) {
        token = next_token(parser);
    }

    if (is_separator(current_token(parser))) {
        printf("Invalid input: Only command allowed after \';\'");

        return;
    }

    command = reconstruct_command(parser);

    cpid = fork();
    if (cpid < 0) {
        exit(-1);
    }

    if (cpid > 0) {
        wait(NULL);
        exit(EXIT_SUCCESS);
    } else {
        execvp(command[0], command);
    }
}