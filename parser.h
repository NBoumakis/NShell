#include <stdlib.h>

#ifndef PARSER_H
#define PARSER_H

typedef struct sh_parser *sh_parser_t;

/* Creates and returns a new parser */
sh_parser_t new_parser();

/* Parses the read command or series of commands */
void input_parse(sh_parser_t, char *, size_t);

/* Clears the parser and its fields allowing it to be reused */
void clear_parser();

/* Clears and deallocates the parser */
void delete_parser();

#endif