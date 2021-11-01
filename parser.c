#include "parser.h"

struct sh_parser {
    char **tokens;       /* Hold each token as a string */
    int cur_token_index; /* Hold the index of the token last returned */
    int token_count;     /* Hold the total number of tokens */
};