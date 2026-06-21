#include "../header/ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Expr *expr_new_ident(char *ident) {
    Expr *ex = malloc(sizeof(Expr));
    if(!ex) {
        printf("expr_new_ident: Failed to create the expression\n");
        return NULL;
    }

    ex->type = EXPR_IDENT;
    ex->ident = strdup(ident);
    if(!ex->ident) {
        printf("expr_new_ident: Failed to create the ident value\n");
        free(ex);
        return NULL;
    }

    return ex;
}

Expr *expr_new_int(int num) {
    Expr *ex = malloc(sizeof(Expr));
    if(!ex) {
        printf("expr_new_int: Failed to create the expression\n");
        return NULL;
    }

    ex->type = EXPR_INT;
    ex->int_value = num;

    return ex;
}

Expr *expr_new_str(char *str) {
    Expr *ex = malloc(sizeof(Expr));
    if(!ex) {
        printf("expr_new_str: Failed to create the expression\n");
        return NULL;
    }

    ex->type = EXPR_STR;
    ex->str_value = strdup(str);
    if(!ex->str_value) {
        printf("expr_new_str: Failed to create the string value\n");
        free(ex);
        return NULL;
    }

    return ex;
}

Expr *expr_new_read(char *promt) {
    Expr *ex = malloc(sizeof(Expr));
    if(!ex) {
        printf("expr_new_read: Failed to create the expression\n");
        return NULL;
    }

    ex->type = EXPR_READ;
    ex->read_var.prompt = strdup(promt);
    if(!ex->str_value) {
        printf("expr_new_str: Failed to create the string value\n");
        free(ex);
        return NULL;
    }

    return ex;
}

Expr *expr_new_bin(Expr *left, Binop_type op, Expr *right) {
    Expr *ex = malloc(sizeof(Expr));
    if(!ex) {
        printf("expr_new_read: Failed to create the expression\n");
        return NULL;
    }

    ex->type = EXPR_BIN;
    ex->bin.left = left;
    ex->bin.op = op;
    ex->bin.right = right;

    return ex;
}

Expr *expr_copy_expr(Expr *ex) {
    switch(ex->type) {
        case EXPR_INT:
            return expr_new_int(ex->int_value);
        case EXPR_IDENT:
            return expr_new_ident(ex->ident);
        case EXPR_STR:
            return expr_new_str(ex->str_value);
        case EXPR_READ:
            return expr_new_read(ex->read_var.prompt);
        case EXPR_BIN:
            return expr_new_bin(expr_copy_expr(ex->bin.left), ex->bin.op, expr_copy_expr(ex->bin.right));
    }
}

void expr_destroy(Expr *ex) {
    if(!ex) return;

    switch(ex->type) {
        case EXPR_INT: 
            break;
        case EXPR_IDENT:
            free(ex->ident);
            break;
        case EXPR_STR:
            free(ex->str_value);
            break;
        case EXPR_READ:
            free(ex->read_var.prompt);
            break;
        case EXPR_BIN:
            expr_destroy(ex->bin.left);
            expr_destroy(ex->bin.right);
            break;
    }

    free(ex);
}

Condition *cond_new(Expr *left, Cond_type type, Expr *right) {
    Condition *new_cond = malloc(sizeof(Condition));
    if(!new_cond) {
        printf("cond_new: Failed to create the condition\n");
        return NULL;
    }

    new_cond->left = left;
    new_cond->type = type;
    new_cond->right = right;

    return new_cond;
}

void cond_destroy(Condition *cond) {
    expr_destroy(cond->left);
    expr_destroy(cond->right);
    free(cond);
}

Ast *ast_new_put(char *varname, Expr *ex) {
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

Ast *ast_new_show(Expr *ex) {
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

Ast *ast_new_if(Condition *cond, Ast *body) {
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

Ast *ast_new_while(Condition *cond, Ast *body) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast)  {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_WHILE;
    new_ast->data.while_stmt.cond = cond;
    new_ast->data.while_stmt.body = body;

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
            case AST_WHILE:
                cond_destroy(temp->data.while_stmt.cond);
                ast_destroy(temp->data.while_stmt.body);
                break;
        }

        free(temp);
    }
}
