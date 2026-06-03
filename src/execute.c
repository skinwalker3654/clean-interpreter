#include "../header/execute.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void execute_put(Ast *ast, Variable_list *list) {
    char buffer[256];
    switch(ast->data.put.expr.type) {
        case EXPR_READ:
            printf("%s",ast->data.put.expr.read_var.prompt);
            fgets(buffer,sizeof(buffer),stdin);
            buffer[strcspn(buffer,"\n")] = '\0';

            Expr new_expr = expr_new_str(buffer);
            push_variable_value(list, ast->data.put.varname, new_expr);
            expr_destroy(new_expr);
            break;
        default:
            push_variable_value(list, ast->data.put.varname, ast->data.put.expr);
            break;
    }
}

void execute_show(Ast *ast, Variable_list *list) {
    int index;
    switch(ast->data.showText.expr.type) {
        case EXPR_STR: 
            printf("%s\n",ast->data.showText.expr.str_value);
            break;
        case EXPR_IDENT:
            index = get_variable_value(list, ast->data.showText.expr.ident);
            if(index == -1) {
                printf("Error: Variable %s not found\n",ast->data.put.varname);
                return;
            }

            if(list->vars[index].ex.type == EXPR_INT)
                printf("%d\n",list->vars[index].ex.int_value);
            else
                printf("%s\n",list->vars[index].ex.str_value);
            break;
        default:
            return;
    }
}

void execute_program(Ast *ast, Variable_list *list) {
    while(ast != NULL) {
        switch(ast->type) {
            case AST_SHOW:
                execute_show(ast, list);
                break;
            case AST_PUT:
                execute_put(ast, list);
                break;
        }

        ast = ast->next;
    }
}
