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
    if (ps->current->type != type) {
        printf("Parse error: %s\n", msg);
        ps->had_error = 1;
        return 0;
    }

    advance(ps);
    return 1;
}

Value parser_parse_value(Parser *ps) {
    Value v = {0};

    if (ps->current->type == TOK_NUM) {
        v = val_new_int(atol(ps->current->value));
        advance(ps);
        return v;
    }

    if (ps->current->type == TOK_STR) {
        v = val_new_str(strdup(ps->current->value));
        advance(ps);
        return v;
    }

    if (ps->current->type == TOK_IDENT) {
        v = val_new_ident(strdup(ps->current->value));
        advance(ps);
        return v;
    }

    printf("Parse error: invalid value '%s'\n", ps->current->value);
    ps->had_error = 1;

    return v;
}

Ast *parser_parse_put(Parser *ps) {
    if(!consum(ps,TOK_PUT,"expected 'put'"))
        return NULL;

    Value v = parser_parse_value(ps);

    if(!consum(ps,TOK_ON,"expected 'on'"))
        return NULL;

    if(ps->current->type != TOK_IDENT) {
        printf("Parse error: expected variable name\n");
        ps->had_error = 1;
        return NULL;
    }

    char *name = strdup(ps->current->value);
    if(!name) return NULL;

    advance(ps);

    if(!consum(ps,TOK_SEMI,"expected ';'"))
        return NULL;

    Ast *new_put = ast_new_put(name,v);
    free(name);

    return new_put;
}

Ast *parser_parse_show(Parser *ps) {
    if(!consum(ps,TOK_SHOW_TEXT,"expected 'showText'\n"))
        return NULL;

    if(!consum(ps,TOK_LPAR,"expected '('")) 
        return NULL;

    Value v = parser_parse_value(ps);

    if(!consum(ps,TOK_RPAR,"expected ')'")) 
        return NULL;

    if(!consum(ps,TOK_SEMI,"expected ';'")) 
        return NULL;
    
    Ast *new_show = ast_new_show(v);
    if(!new_show) return NULL;

    return new_show;
}

Ast *parser_parse_stmt(Parser *ps) {
    if(ps->current->type == TOK_PUT) {
        return parser_parse_put(ps);
    } else if(ps->current->type == TOK_SHOW_TEXT) {
        return parser_parse_show(ps);
    } else {
        printf("Error: Invalid value to start '%s'\n",ps->current->value);
        ps->had_error = 1;
        advance(ps);
        return NULL;
    }
}

Ast *parser_parse_program(Parser *ps) {
    Ast *head = NULL;
    while(ps->current->type != TOK_EOF) {
        Ast *new = parser_parse_stmt(ps);
        ast_append(&head, new);
    }

    return head;
}
