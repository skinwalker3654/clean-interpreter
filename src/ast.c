#include "../header/ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Expr expr_new_ident(char *ident) {
    Expr ex = {EXPR_IDENT,.ident=strdup(ident)};
    return ex;
}

Expr expr_new_int(int num) {
    Expr exal = {EXPR_INT,.int_value=num};
    return exal;
}

Expr expr_new_str(char *str) {
    Expr ex = {EXPR_STR,.str_value=strdup(str)};
    return ex;
}

Expr expr_new_read(char *prompt) {
   Expr ex = {EXPR_READ,.read_var.prompt=strdup(prompt)};
   return ex;
}

void expr_destroy(Expr ex) {
    switch(ex.type) {
        case EXPR_IDENT:
            free(ex.ident);
            return;
        case EXPR_STR:
            free(ex.str_value);
            return;
        case EXPR_READ:
            free(ex.read_var.prompt);
            return;
        default:
            return;
    }
}

Expr expr_copy_expr(Expr ex) {
    switch(ex.type) {
        case EXPR_STR:
            return expr_new_str(ex.str_value);
        case EXPR_IDENT:
            return expr_new_ident(ex.ident);
        default:
            return ex;
    }
}

Condition cond_new(Expr left, Cond_type type, Expr right) {
    Condition new = { type,left, right};
    return new;
}

void cond_destroy(Condition cond) {
    expr_destroy(cond.left);
    expr_destroy(cond.right);
}

Ast *ast_new_put(char *varname, Expr ex) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast) {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_PUT;
    new_ast->data.put.expr = ex;

    new_ast->data.put.varname = strdup(varname);
    if(!new_ast->data.put.varname) {
        printf("Failed to create the exariable ast\n");
        free(new_ast);
        return NULL;
    }
    
    new_ast->next = NULL;
    return new_ast;
}

Ast *ast_new_show(Expr ex) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast) {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_SHOW;
    new_ast->data.showText.expr = ex;

    new_ast->next = NULL;
    return new_ast;
}

Ast *ast_new_if(Condition cond, Ast *body) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast)  {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_IF;
    new_ast->data.if_stmt.cond = cond;
    new_ast->data.if_stmt.body = body;

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
    expr_destroy(ast->data.put.expr);
}

static void ast_destroy_show(Ast *ast) {
    expr_destroy(ast->data.showText.expr);
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
            case AST_IF:
                cond_destroy(temp->data.if_stmt.cond);
                ast_destroy(temp->data.if_stmt.body);
                break;
        }

        free(temp);
    }
}
