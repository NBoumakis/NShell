/*
 * Development: Nikos Boumakis, 4346
 * Email: csd4346@csd.uoc.gr
 */

#include "parser.h"
#include "shell.h"
#include <stdio.h>
#include <string.h>

int main() {
    command_sequence_t command_seq = NULL;
    char *lineptr = NULL;
    size_t lineLength = 0;
    size_t read;

    while (1) {
        prompt_string();

        read = getline(&lineptr, &lineLength, stdin);

        if (read == -1) {
            free(lineptr);
            printf("Reading command failed, exiting now");
            exit(EXIT_FAILURE);
        }

        command_seq = parse(lineptr, read - 1, command_seq);
        execute(command_seq);

        clear_command_sequence(command_seq);

        free(lineptr);
        lineptr = NULL;
        lineLength = 0;
    }

    free_command_sequence(command_seq);

    return 0;
}