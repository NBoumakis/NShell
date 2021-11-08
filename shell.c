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

    printf("%s%s%s$ ", user, "@cs345sh/", dir);
}

void execute(command_sequence_t command_seq) {
    command_simple_t command;
    pid_t cpid;
    int tmp_input = dup(STDIN_FILENO),   /* tmp_input = stdin */
        tmp_output = dup(STDOUT_FILENO), /* tmp_output = stdout */
        input, output, previous_pipe_out;
    int pipefd[2];

    command = get_current_simple_command(command_seq);

    while (command) {
        if (strcmp(get_command_and_arguments(command)[0], "cd") == 0) {
            chdir(get_command_and_arguments(command)[1]);
        } else if (strcmp(get_command_and_arguments(command)[0], "exit") == 0) {
            exit(0);
        } else {
            if (get_input_filename(command)) {
                input = open(get_input_filename(command), O_RDONLY);
                dup2(input, STDIN_FILENO);
                close(input);
            }

            if (get_output_filename(command)) {
                if (get_output_type(command) == OVERWRITE) {
                    output = open(get_output_filename(command),
                                  O_WRONLY | O_TRUNC | O_CREAT,
                                  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
                } else {
                    output = open(get_output_filename(command),
                                  O_WRONLY | O_APPEND | O_CREAT,
                                  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
                }

                dup2(output, STDOUT_FILENO);
                close(output);
            }

            if (get_pipe_input(command)) {
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);
            }

            if (get_pipe_output(command)) {
                pipe(pipefd);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
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
            } else {
                execvp(get_command_and_arguments(command)[0],
                       get_command_and_arguments(command));
            }
        }

        command = get_next_simple_command(command_seq);
    }
}