#include "../header/ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Value val_new_ident(char *ident) {
    Value val = {VAL_IDENT,.data.ident=ident};
    return val;
}

Value val_new_int(int num) {
    Value val = {VAL_INT,.data.int_value=num};
    return val;
}

Value val_new_str(char *str) {
    Value val = {VAL_STR,.data.str_value=str};
    return val;
}

void val_destroy(Value v) {
    switch(v.type) {
        case VAL_IDENT:
            free(v.data.ident);
            return;
        case VAL_STR:
            free(v.data.str_value);
            return;
        default:
            return;
    }
}

Ast *ast_new_put(char *varname, Value v) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast) {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_PUT;
    new_ast->data.put.value = v;

    new_ast->data.put.varname = strdup(varname);
    if(!new_ast->data.put.varname) {
        printf("Failed to create the variable ast\n");
        free(new_ast);
        return NULL;
    }
    
    new_ast->next = NULL;
    return new_ast;
}

Ast *ast_new_show(Value v) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast) {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_SHOW;
    new_ast->data.showText.value = v;

    new_ast->next = NULL;
    return new_ast;
}

void ast_append(Ast **head, Ast *node) {
    if(*head == NULL) {
        *head = node;
        return;
    }

    Ast *temp = *head;
    while(temp->next != NULL)
        temp = temp->next;
    temp->next = node;
}

/* destroy API */
static void ast_destroy_put(Ast *ast) {
    free(ast->data.put.varname);
    val_destroy(ast->data.put.value);
}

static void ast_destroy_show(Ast *ast) {
    val_destroy(ast->data.showText.value);
}

void ast_destroy(Ast *ast) {
    if(!ast) return;

    Ast *temp;
    while(ast != NULL) {
        temp = ast;
        ast = ast->next;
        switch(temp->type) {
            case AST_PUT:
                ast_destroy_put(temp);
                break;
            case AST_SHOW:
                ast_destroy_show(temp);
                break;
        }

        free(temp);
    }
}
