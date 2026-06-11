#include "../header/execute.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int execute_put(Ast *ast, Variable_list *list) {
    char *buffer;
    size_t size = 0;
    switch(ast->data.put.expr.type) {
        case EXPR_READ:
            printf("%s",ast->data.put.expr.read_var.prompt);
            getline(&buffer,&size,stdin);
            buffer[strcspn(buffer,"\n")] = '\0';

            Expr new_expr = expr_new_str(buffer);
            push_variable_value(list, ast->data.put.varname, new_expr);
            expr_destroy(new_expr);
            free(buffer);
            return 0;
        default:
            push_variable_value(list, ast->data.put.varname, ast->data.put.expr);
            return 0;
    }
}

int execute_show(Ast *ast, Variable_list *list) {
    int index;
    switch(ast->data.showText.expr.type) {
        case EXPR_STR: 
            printf("%s\n",ast->data.showText.expr.str_value);
            return 0;
        case EXPR_IDENT:
            index = get_variable_index(list, ast->data.showText.expr.ident);
            if(index == -1) {
                printf("Error showText: Variable '%s' not found\n",ast->data.showText.expr.ident);
                return -1;
            }

            if(list->vars[index].ex.type == EXPR_INT)
                printf("%d\n",list->vars[index].ex.int_value);
            else
                printf("%s\n",list->vars[index].ex.str_value);
            return 0;
        default:
            return -1;
    }
}

int execute_program(Ast *ast, Variable_list *list) {
    while(ast != NULL) {
        switch(ast->type) {
            case AST_SHOW:
                if(execute_show(ast, list)==-1) return -1;
                break;
            case AST_PUT:
                if(execute_put(ast, list)==-1) return -1;
                break;
        }

        ast = ast->next;
    }

    return 0;
}
