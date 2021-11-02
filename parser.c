#include "parser.h"
#include <ctype.h>
#include <string.h>

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

static int is_separator(char input) {
    if (input == '|' || input == '<' || input == '>' || input == ';') {
        return 1;
    }

    return 0;
}

static int count_tokens(char *input, size_t input_length) {
    int count = 0;
    int i;

    for (i = 0; i < input_length; i++) {
        while (isspace(input[i])) {
            i++;
        }

        if (!is_separator(input[i])) {
            while (i < input_length && !is_separator(input[i]))
                i++;

            count++;
        }
        if (is_separator(input[i])) {
            if (i + 1 < input_length && input[i] == '>' && input[i + 1] == '>') {
                i++;
            }
            count++;
        }
    }

    return count;
}

void input_parse(sh_parser_t parser, char *input, size_t input_length) {
    int count = 0;
    char **token_arr;
    char *input_tmp = input;
    size_t i = 0, token_size, token_start, token_end;

    count = count_tokens(input, input_length);
    parser->token_count = count;
    parser->cur_token_index = 0;

    if (count == 0) {
        parser->tokens = NULL;

        return;
    }

    token_arr = malloc(sizeof(char *) * count);

    token_start = 0;
    for (i = 0; i < count; i++) {
        while (isspace(input[token_start]))
            token_start++;

        token_end = token_start;

        if (!is_separator(input[token_start])) {
            while (token_end < input_length && !is_separator(input[token_end]))
                token_end++;

            while (isspace(input[--token_end]))
                ;
        } else {
            if (token_start + 1 < input_length && input[token_start] == '>' &&
                input[token_start + 1] == '>') {
                token_end++;
            }
        }

        token_size = token_end - token_start + 1;
        token_arr[i] = malloc((token_size + 1));

        strncpy(token_arr[i], &input[token_start], token_size);
        token_arr[i][token_size] = '\0';

        token_start = token_end + 1;
    }

    parser->tokens = token_arr;
}

void clear_parser(sh_parser_t parser) {
    int i;

    for (i = 0; i < parser->token_count; i++) {
        free(parser->tokens[i]);
        parser->tokens[i] = NULL;
    }

    free(parser->tokens);
    parser->tokens = NULL;

    parser->token_count = 0;
    parser->cur_token_index = 0;
}

void delete_parser(sh_parser_t parser) {
    clear_parser(parser);
    free(parser);
}

char *next_token(sh_parser_t parser) {
    parser->cur_token_index++;
    return current_token(parser);
}

char *previous_token(sh_parser_t parser) {
    parser->cur_token_index--;
    return current_token(parser);
}

char *current_token(sh_parser_t parser) {
    if (parser->cur_token_index < 0 ||
        parser->cur_token_index >= parser->token_count) {
        return NULL;
    }

    return parser->tokens[parser->cur_token_index];
}
