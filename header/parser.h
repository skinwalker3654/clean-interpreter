#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "ast.h"

typedef struct {
    Lexer *lx;
    Token *current;
    int had_error;
} Parser;

Parser *parser_init(char *source);
void parser_destroy(Parser *ps);

Value parser_parse_value(Parser *ps);
Ast *parser_parse_put(Parser *ps);
Ast *parser_parse_show(Parser *ps);
Ast *parser_parse_stmt(Parser *ps);
Ast *parser_parse_program(Parser *ps);

#endif
