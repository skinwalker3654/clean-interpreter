#include "../header/parser.h"
#include "../header/token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* OWNER - SHIP*/
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

/* PARSER - API */
static int match(Parser *ps, Tok_type type) {
    if(!ps) return 0;
    return ps->current->type == type;
}

static int advance(Parser *ps) {
    token_destroy(ps->current);
    ps->current = token_get_next(ps->lx);
    if(ps->current == NULL) return 0;
    return 1;
}

static int consum(Parser *ps, Tok_type type, const char *msg) {
    if(!match(ps,type)) {
        printf("Line %d: Parse error: %s\n",ps->lx->line,msg);
        return 0;
    }

    if(advance(ps)==0) return 0;
    return 1;
}

static Expr *parse_additive(Parser *ps);
static Expr *parse_multiplicative(Parser *ps);
static Expr *parse_primary(Parser *ps);

static Expr *parse_primary(Parser *ps) {
    if(match(ps, TOK_NUM)) {
        Expr *ex = expr_new_int(atol(ps->current->value));
        advance(ps);
        return ex;
    }

    if(match(ps,TOK_LPAR)) {
        advance(ps);
        Expr *ex = parser_parse_expr(ps);
        if(!consum(ps,TOK_RPAR,"Expected ')'")) {
            expr_destroy(ex);
            return NULL;
        }

        return ex;
    }

    if(match(ps,TOK_IDENT)) {
        Expr *ex = expr_new_ident(ps->current->value);
        advance(ps);
        return ex;
    }

    if(match(ps,TOK_STR)) {
        Expr *ex = expr_new_str(ps->current->value);
        advance(ps);
        return ex;
    }

    if(match(ps,TOK_READ_VAR)) {
        advance(ps);
        if(!consum(ps,TOK_LPAR,"Expected '('"))
            return NULL;

        if(!match(ps,TOK_STR)) {
            printf("Error line %d: Expected message on readVar\n",ps->lx->line);
            return NULL;
        }

        char *prompt = strdup(ps->current->value);
        advance(ps);

        if(!consum(ps,TOK_RPAR,"Expected ')'")) {
            free(prompt);
            return NULL;
        }

        Expr *ex = expr_new_read(prompt);
        free(prompt);
        return ex;
    }

    printf("Error line %d: Expected an expression\n",ps->lx->line);
    return NULL;
}

static Expr *parse_additive(Parser *ps) {
    Expr *left = parse_multiplicative(ps);
    if(!left) return NULL;

    while(match(ps,TOK_PLUS) || match(ps,TOK_MINUS)) {
        Binop_type op;
        if(match(ps,TOK_PLUS)) {
            op = OP_ADD;
        } else if(match(ps,TOK_MINUS)) {
            op = OP_SUB;
        } else {
            printf("Error line %d: Invalid arithmetic symbol '%c'\n",ps->lx->line,ps->lx->source[ps->lx->pos-1]);
            expr_destroy(left);
            return NULL;
        }

        advance(ps);
        Expr *right = parse_multiplicative(ps);
        if(!right) {
            expr_destroy(left);
            return NULL;
        }

        left = expr_new_bin(left, op, right);
        if(!left) {
            expr_destroy(right);
            return NULL;
        }
    }

    return left;
}

static Expr *parse_multiplicative(Parser *ps) {
    Expr *left = parse_primary(ps);
    if(!left) return NULL;

    while(match(ps,TOK_STAR) || match(ps,TOK_SLASH)) {
        Binop_type op;
        if(match(ps,TOK_STAR)) {
            op = OP_MUL;
        } else if(match(ps,TOK_SLASH)) {
            op = OP_DIV;
        } else {
            printf("Error line %d: Invalid arithmetic symbol '%c'\n",ps->lx->line,ps->lx->source[ps->lx->pos-1]);
            expr_destroy(left);
            return NULL;
        }

        advance(ps);
        Expr *right = parse_primary(ps);
        if(!right) {
            expr_destroy(left);
            return NULL;
        }

        left = expr_new_bin(left, op, right);
        if(!left) {
            expr_destroy(right);
            return NULL;
        }
    }

    return left;
}

Expr *parser_parse_expr(Parser *ps) {
    return parse_additive(ps);
}

/* COND - API */
static Cond_type tok_type_to_cond(Parser *ps) {
    switch(ps->current->type) {
        case TOK_EQEQ:
            return COND_EQEQ;
        case TOK_GE:
            return COND_GE;
        case TOK_GT:
            return COND_GT;
        case TOK_LE:
            return COND_LE;
        case TOK_LT:
            return COND_LT;
        case TOK_NOT_EQ:
            return COND_NOT_EQ;
        default:
            return -1;
    }
}

Condition *parser_parse_cond(Parser *ps) {
    Condition *cond;

    Expr *left = parser_parse_expr(ps);
    if(!left) {
        expr_destroy(left);
        return NULL;
    }

    if(ps->current->type != TOK_EQEQ 
        && ps->current->type != TOK_GE
        && ps->current->type != TOK_GT 
        && ps->current->type != TOK_LE
        && ps->current->type != TOK_LT
        && ps->current->type != TOK_NOT_EQ) {
        printf("Parser error line %d: invalid condition type. ",ps->lx->line);
        expr_destroy(left);
        return cond;
    }

    Cond_type type = tok_type_to_cond(ps);
    advance(ps);

    Expr *right = parser_parse_expr(ps);
    if(!right) {
        expr_destroy(left);
        expr_destroy(right);
        return NULL;
    }

    return cond_new(left, type, right);
}

/* AST - API */
Ast *parser_parse_put(Parser *ps) {
    if(!consum(ps,TOK_PUT,"expected 'put'"))
        return NULL;

    Expr *ex = parser_parse_expr(ps);
    if(!ex) return NULL;

    if(!consum(ps,TOK_ON,"expected 'on'")) {
        expr_destroy(ex);
        return NULL;
    }

    if(!match(ps,TOK_IDENT)) {
        printf("Parse error: expected variable name\n");
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

    Expr *ex = parser_parse_expr(ps);
    if(!ex) return NULL;

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

Ast *parser_parse_stmt(Parser *ps);

Ast *parser_parse_if(Parser *ps) {
    if(!consum(ps,TOK_IF,"expected 'if'"))
        return NULL;
    
    Condition *cond = parser_parse_cond(ps);

    if(!consum(ps,TOK_LBRA,"expected '{'")) {
        cond_destroy(cond);
        return NULL;
    }

    Ast *body = NULL;
    while(ps->current->type != TOK_RBRA) {
        Ast *stmt = parser_parse_stmt(ps);
        if(!stmt){
            ast_destroy(body);
            cond_destroy(cond);
            return NULL;
        }

        ast_append(&body,stmt);
    }

    if(!consum(ps,TOK_RBRA,"expected '}'")) {
        ast_destroy(body);
        cond_destroy(cond);
        return NULL;
    }

    return ast_new_if(cond,body);
}

Ast *parser_parse_while(Parser *ps) {
    if(!consum(ps,TOK_WHILE,"expected 'while'"))
        return NULL;
    
    Condition *cond = parser_parse_cond(ps);

    if(!consum(ps,TOK_LBRA,"expected '{'")) {
        cond_destroy(cond);
        return NULL;
    }

    Ast *body = NULL;
    while(ps->current->type != TOK_RBRA) {
        Ast *stmt = parser_parse_stmt(ps);
        if(!stmt){
            ast_destroy(body);
            cond_destroy(cond);
            return NULL;
        }

        ast_append(&body, stmt);
    }

    if(!consum(ps,TOK_RBRA,"expected '}'")) {
        ast_destroy(body);
        cond_destroy(cond);
        return NULL;
    }

    return ast_new_while(cond,body);

}

Ast *parser_parse_stmt(Parser *ps) {
    if(ps->current->type == TOK_PUT) {
        return parser_parse_put(ps);
    } else if(ps->current->type == TOK_SHOW_TEXT) {
        return parser_parse_show(ps);
    } else if(ps->current->type == TOK_IF) {
        return parser_parse_if(ps);
    } else if(ps->current->type == TOK_WHILE) {
        return parser_parse_while(ps);
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
