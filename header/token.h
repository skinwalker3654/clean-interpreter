#ifndef TOKEN_H
#define TOKEN_H

#include "lexer.h"

typedef enum {
    TOK_PUT,
    TOK_ON,
    TOK_NUM,
    TOK_STR,
    TOK_IDENT,
    TOK_SHOW_TEXT,
    TOK_READ_VAR,
    TOK_SEMI,
    TOK_LPAR,
    TOK_RPAR,
    TOK_EOF,
    TOK_ERR,
} Tok_type;

typedef struct {
    Tok_type type;
    char *value;
} Token;

Token *token_create(Tok_type type, char *value);
void token_destroy(Token *tk);
Token *token_get_next(Lexer *lx);

#endif
