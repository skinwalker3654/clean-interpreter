#ifndef TOKEN_H
#define TOKEN_H

#include "lexer.h"

typedef enum {
    /* keywords */
    TOK_PUT,
    TOK_ON,
    TOK_IF,

    /* values */
    TOK_NUM,
    TOK_STR,
    TOK_IDENT,

    /* functions */
    TOK_SHOW_TEXT,
    TOK_READ_VAR,

    /* symbols */
    TOK_SEMI,
    TOK_LPAR,
    TOK_RPAR,
    TOK_LBRA,
    TOK_RBRA,

    /* conditions */
    TOK_EQEQ,
    TOK_GE,
    TOK_GT,
    TOK_LE,
    TOK_LT,
    TOK_NOT_EQ,

    /* extra */
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
