#include "../header/parser.h"
#include "../header/token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Parser *parser_init(char *source) {
    Parser *new_parser = malloc(sizeof(Parser));
    if(!new_parser) {
        printf("Error: Failed to create the parser\n");
        return NULL;
    }

    new_parser->lx = lexer_create(source);
    if(!new_parser->lx) {
        printf("Failed to initialize the lexer\n");
        free(new_parser);
        return NULL;
    }

    new_parser->current = token_get_next(new_parser->lx);
    if(!new_parser->current) {
        printf("Failed to get the first token\n");
        lexer_destroy(new_parser->lx);
        free(new_parser);
        return NULL;
    }

    return new_parser;
}

void parser_destroy(Parser *ps) {
    if(!ps) return;

    lexer_destroy(ps->lx);
    token_destroy(ps->current);

    free(ps);
    ps = NULL;
}

static void advance(Parser *ps) {
    token_destroy(ps->current);
    ps->current = token_get_next(ps->lx);
}

static int consum(Parser *ps, Tok_type type, const char *msg) {
    if(ps->current->type != type) {
        printf("Line %d: Parse error: %s\n",ps->lx->line,msg);
        return 0;
    }

    advance(ps);
    return 1;
}

Expr parser_parse_expr(Parser *ps) {
    Expr ex = {0};

    if(ps->current->type == TOK_NUM) {
        ex = expr_new_int(atol(ps->current->value));
        advance(ps);
        return ex;
    }

    if(ps->current->type == TOK_STR) {
        ex = expr_new_str(ps->current->value);
        advance(ps);
        return ex;
    }

    if(ps->current->type == TOK_IDENT) {
        ex = expr_new_ident(ps->current->value);
        advance(ps);
        return ex;
    }

    if(ps->current->type == TOK_READ_VAR) {
        advance(ps);
        if(!consum(ps,TOK_LPAR,"expected '('")) {
            return ex;
        }
        
        if(ps->current->type != TOK_STR) {
            printf("Expected string inside of \" \"\n");
            return ex;
        }

        ex = expr_new_read(ps->current->value);
        advance(ps);

        if(!consum(ps,TOK_RPAR,"expected ')'")) {
            return ex;
        }
        
        return ex;
    }

    printf("Parse error: inexprid exalue '%s'\n", ps->current->value);
    return ex;
}

Ast *parser_parse_put(Parser *ps) {
    if(!consum(ps,TOK_PUT,"expected 'put'"))
        return NULL;

    Expr ex = parser_parse_expr(ps);

    if(!consum(ps,TOK_ON,"expected 'on'")) {
        expr_destroy(ex);
        return NULL;
    }

    if(ps->current->type != TOK_IDENT) {
        printf("Parse error: expected exariable name\n");
        expr_destroy(ex);
        return NULL;
    }

    char *name = strdup(ps->current->value);
    if(!name) return NULL;

    advance(ps);

    if(!consum(ps,TOK_SEMI,"expected ';'")) {
        free(name);
        expr_destroy(ex);
        return NULL;
    }

    Ast *new_put = ast_new_put(name,ex);
    if(!new_put) {
        free(name);
        expr_destroy(ex);
        return NULL;
    }

    free(name);
    return new_put;
}

Ast *parser_parse_show(Parser *ps) {
    if(!consum(ps,TOK_SHOW_TEXT,"expected 'showText'\n"))
        return NULL;

    if(!consum(ps,TOK_LPAR,"expected '('")) 
        return NULL;

    Expr ex = parser_parse_expr(ps);

    if(!consum(ps,TOK_RPAR,"expected ')'")) {
        expr_destroy(ex);
        return NULL;
    }

    if(!consum(ps,TOK_SEMI,"expected ';'"))  {
        expr_destroy(ex);
        return NULL;
    }
    
    Ast *new_show = ast_new_show(ex);
    if(!new_show) {
        expr_destroy(ex);
        return NULL;
    }

    return new_show;
}

Ast *parser_parse_stmt(Parser *ps) {
    if(ps->current->type == TOK_PUT) {
        return parser_parse_put(ps);
    } else if(ps->current->type == TOK_SHOW_TEXT) {
        return parser_parse_show(ps);
    } else {
        printf("Error: Inexprid exalue to start '%s'\n",ps->current->value);
        advance(ps);
        return NULL;
    }
}

Ast *parser_parse_program(Parser *ps) {
    Ast *head = NULL;
    while(ps->current->type != TOK_EOF) {
        Ast *new = parser_parse_stmt(ps);
        if(!new) {
            ast_destroy(head);
            return NULL;
        }

        ast_append(&head, new);
    }

    return head;
}
