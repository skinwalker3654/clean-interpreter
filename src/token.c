#include "../header/token.h"
#include "../header/lexer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

Token *token_create(Tok_type type, char *value) {
    Token *new_tok = malloc(sizeof(Token));
    if(!new_tok) {
        printf("Failed to create the token\n");
        return NULL;
    }

    new_tok->type = type;
    
    if(type != TOK_IDENT && type != TOK_NUM && type != TOK_STR) 
        return new_tok;

    new_tok->value = strdup(value);
    if(!new_tok) {
        printf("Failed to put the value into the token\n");
        free(new_tok);
        return NULL;
    }

    return new_tok;
}

void token_destroy(Token *tk) {
    if(!tk) return;

    if(tk->type == TOK_IDENT || tk->type == TOK_NUM || tk->type == TOK_STR)
        free(tk->value);

    free(tk);
    tk = NULL;
}

static int string_push_char(char **string, char ch) {
    if(!*string) {
        *string = malloc(2);
        if(!*string) return -1;

        (*string)[0] = ch;
        (*string)[1] = '\0';
        return 0;
    }

    int size = strlen(*string);
    char *temp =  realloc(*string,size+2);
    if(!temp) return -1;

    *string = temp;
    (*string)[size] = ch;
    (*string)[size+1] = '\0';

    return 0;
}

static Token *get_ident(Lexer *lx) {
    char *buffer = NULL;
    while(isalnum(lexer_peek(lx)) || lexer_peek(lx) == '_') {
        string_push_char(&buffer,lexer_advance(lx));
    }

    string_push_char(&buffer,'\0');
    if(strcmp(buffer,"put")==0) {
        free(buffer);
        return token_create(TOK_PUT,NULL);
    } else if(strcmp(buffer,"on")==0) {
        free(buffer);
        return token_create(TOK_ON,NULL);
    } else if(strcmp(buffer,"showText")==0) {
        free(buffer);
        return token_create(TOK_SHOW_TEXT,NULL);
    } else if(strcmp(buffer,"readVar")==0) {
        free(buffer);
        return token_create(TOK_READ_VAR,NULL);
    } else if(strcmp(buffer,"if")==0) {
        free(buffer);
        return token_create(TOK_IF,NULL);
    } else if(strcmp(buffer,"while")==0) {
        free(buffer);
        return token_create(TOK_WHILE,NULL);
    } else { 
        Token *tk = token_create(TOK_IDENT,buffer);
        free(buffer);
        return tk;
    }
}

static Token *get_number(Lexer *lx) {
    char *buffer = NULL;

    while(isdigit(lexer_peek(lx))) {
        string_push_char(&buffer, lexer_advance(lx));
    }

    string_push_char(&buffer,'\0');

    Token *tk = token_create(TOK_NUM,buffer);
    free(buffer);

    return tk;
}

static Token *get_string(Lexer *lx) {
    lexer_advance(lx);

    char *buffer = NULL;
    while(lexer_peek(lx) != '"' && lexer_peek(lx) != '\0') {
        string_push_char(&buffer, lexer_advance(lx));
    }

    if(lexer_peek(lx) == '\0') {
        printf("Error %d: String %s never closed",lx->line,buffer);
        return NULL;
    }

    lexer_advance(lx);
    string_push_char(&buffer,'\0');

    Token *tk = token_create(TOK_STR,buffer);
    free(buffer);

    return tk;
}

/* TOKEN_GET_NEXT FUNCTION */
Token *token_get_next(Lexer *lx) {
    lexer_skip_ws(lx);
    char current = lexer_peek(lx);

    if(current == '\0') {
        return token_create(TOK_EOF,NULL);
    }

    if(current == '#') {
        while(lexer_peek(lx) != '\n') lexer_advance(lx);
        return token_get_next(lx);
    }

    if(isalpha(current)) {
        return get_ident(lx);
    }

    if(isdigit(current)) {
        return get_number(lx);
    }

    if(current == '"') {
        return get_string(lx);
    }

    char doubles[3] = {current,lx->source[lx->pos+1],'\0'};
    if(strcmp(doubles,"==")==0) {
        lexer_advance(lx);
        lexer_advance(lx);
        return token_create(TOK_EQEQ,NULL);
    } else if(strcmp(doubles,">=")==0) {
        lexer_advance(lx);
        lexer_advance(lx);
        return token_create(TOK_GE,NULL);
    } else if(strcmp(doubles,"<=")==0) {
        lexer_advance(lx);
        lexer_advance(lx);
        return token_create(TOK_LE,NULL);
    } else if(strcmp(doubles,"!=")==0) {
        lexer_advance(lx);
        lexer_advance(lx);
        return token_create(TOK_NOT_EQ,NULL);
    }

    char symbol[2] = {current,'\0'};
    lexer_advance(lx);

    switch(symbol[0]) {
        case ';': return token_create(TOK_SEMI,NULL);
        case '(': return token_create(TOK_LPAR,NULL);
        case ')': return token_create(TOK_RPAR,NULL);
        case '{': return token_create(TOK_LBRA,NULL);
        case '}': return token_create(TOK_RBRA,NULL);
        case '<': return token_create(TOK_LT,NULL);
        case '>': return token_create(TOK_GT,NULL);
    }

    printf("Error %d: Invalid character '%s'\n",lx->line,symbol);
    return token_create(TOK_ERR,NULL);
}
