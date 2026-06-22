#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "ast.h"

typedef struct {
    Lexer *lx;
    Token *current;
} Parser;

Parser *parser_init(char *source);
void parser_destroy(Parser *ps);

Expr *parser_parse_expr(Parser *ps);
Condition *parser_parse_cond(Parser *ps);

Ast *parser_parse_put(Parser *ps);
Ast *parser_parse_show(Parser *ps);
Ast *parser_parse_if(Parser *ps);
Ast *parser_parse_while(Parser *ps);
Ast *parser_parse_var_assign(Parser *ps);

Ast *parser_parse_stmt(Parser *ps);
Ast *parser_parse_program(Parser *ps);

#endif
