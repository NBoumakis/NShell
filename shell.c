#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define DIR_MAX_LENGTH 1024

void prompt_string() {
    char *user = getlogin();
    char dir[DIR_MAX_LENGTH];

    getcwd(dir, DIR_MAX_LENGTH);

    printf("%s%s%s", user, "@cs345sh/", dir);
}