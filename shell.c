#include "shell.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
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

void execute(command_sequence_t command_seq) {
    command_simple_t command;
    pid_t cpid;
    int tmp_input = dup(STDIN_FILENO),   /* tmp_input = stdin */
        tmp_output = dup(STDOUT_FILENO), /* tmp_output = stdout */
        input, output;
    int pipefd[2];

    command = get_current_simple_command(command_seq);

    while (command) {
        if (get_input_filename(command)) {
            input = open(get_input_filename(command), O_RDONLY);
            dup2(input, STDIN_FILENO);
            close(input);
        }

        if (get_output_filename(command)) {
            if (get_output_type(command) == OVERWRITE) {
                output =
                    open(get_output_filename(command), O_WRONLY | O_TRUNC | O_CREAT);
            } else {
                output = open(get_output_filename(command), O_APPEND | O_CREAT);
            }

            dup2(output, STDOUT_FILENO);
            close(output);
        }

        cpid = fork();
        if (cpid < 0) {
            exit(-1);
        }

        if (cpid > 0) {
            wait(NULL);

            dup2(tmp_output, STDOUT_FILENO);
            dup2(tmp_input, STDIN_FILENO);
            close(tmp_input);
            close(tmp_output);

            exit(EXIT_SUCCESS);
        } else {
            execvp(get_command_and_arguments(command)[0],
                   get_command_and_arguments(command));
        }
    }
}