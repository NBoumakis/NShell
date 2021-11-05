#include "parser.h"
#include <ctype.h>
#include <string.h>

struct sh_parser {
    char **tokens;       /* Hold each token as a string */
    int cur_token_index; /* Hold the index of the token last returned */
    int token_count;     /* Hold the total number of tokens */
};

enum parser_states { TEXT, MULTIPLE, PIPE, REDIR_IN, REDIR_OUT, WHITESPACE };

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

enum parser_states next_state(char c) {
    if (c == ';') {
        return MULTIPLE;
    } else if (isspace(c)) {
        return WHITESPACE;
    } else if (c == '|') {
        return PIPE;
    } else if (c == '<') {
        return REDIR_IN;
    } else if (c == '>') {
        return REDIR_OUT;
    } else {
        return TEXT;
    }
}

enum parser_states action_text(char c, int *count) {
    enum parser_states new_state = next_state(c);

    if (new_state != TEXT || c == '\0')
        (*count)++;

    return new_state;
}
enum parser_states action_multiple(char c, int *count) {
    (*count)++;

    return next_state(c);
}
enum parser_states action_pipe(char c, int *count) {
    (*count)++;

    return next_state(c);
}
enum parser_states action_redir_in(char c, int *count) {
    (*count)++;

    return next_state(c);
}
enum parser_states action_redir_out(char c, int *count) {
    if (c == '>')
        return REDIR_OUT;
    else {
        (*count)++;
        return next_state(c);
    }
}

enum parser_states action_whitespace(char c, int *count) { return next_state(c); }

static int count_tokens(char *input, size_t input_length) {
    int count = 0;

    enum parser_states (*actions[6])(char, int *) = {
        action_text,     action_multiple,  action_pipe,
        action_redir_in, action_redir_out, action_whitespace};
    enum parser_states state;

    int i = 0;

    /* Only \0 is present in the input */
    if (input_length == 1) {
        return 0;
    }

    state = next_state(input[0]);

    while (i < input_length) {
        state = actions[state](input[i], &count);
        i++;
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

        if (!is_separator(
                input[token_start])) { /* The current token is not a separator, look
                                          until the first or a space */
            while (token_end < input_length && !is_separator(input[token_end]) &&
                   !isspace(input[token_end]))
                token_end++;

            /* Move the end back by one so token_end points to the last character of
             * the token*/
            token_end--;
        } else {
            if (token_start + 1 < input_length && input[token_start] == '>' &&
                input[token_start + 1] == '>') {
                token_end++;
            }
        }

        token_size = token_end - token_start + 1;
        token_arr[i] = malloc((token_size + 1) * sizeof(char));

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
