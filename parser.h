#include <stdlib.h>

#ifndef PARSER_H
#define PARSER_H

typedef struct sh_parser *sh_parser_t;

/* Creates and returns a new parser */
sh_parser_t new_parser();

/* Parses the read command or series of commands */
void input_parse(sh_parser_t, char *, size_t);

/* Clears the parser and its fields allowing it to be reused */
void clear_parser(sh_parser_t);

/* Clears and deallocates the parser */
void delete_parser(sh_parser_t);

/* Return the next token and move current to be the token forward.
 * Returns NULL if out of bounds. */
char *next_token(sh_parser_t);

/* Return the previous token and move current to be the token backwards.
 * Returns NULL if out of bounds */
char *previous_token(sh_parser_t);

/* Return the current token or NULL if out of bounds */
char *current_token(sh_parser_t);
#endif