#include "parser.h"

struct sh_parser {
    char **tokens;       /* Hold each token as a string */
    int cur_token_index; /* Hold the index of the token last returned */
    int token_count;     /* Hold the total number of tokens */
};

sh_parser_t new_parser() {
    sh_parser_t parser_new = malloc(sizeof(struct sh_parser));

    if (parser_new != NULL) {
        parser_new->cur_token_index = 0;
        parser_new->token_count = 0;
        parser_new->tokens = NULL;
    }

    return parser_new;
}