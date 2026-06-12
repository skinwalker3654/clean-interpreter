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

static int eval_variable(Expr var) {
    if(var.type != EXPR_IDENT && var.type != EXPR_INT) {
        printf("Error: Value on the if statment has an invalid value");
        return 0;
    }

    return  1;
}

int execute_eval_cond(Variable_list *list, Condition cond) {
    if(!eval_variable(cond.left)) return 0;
    if(!eval_variable(cond.right)) return 0;

    int left_val;
    if(cond.left.type == EXPR_INT) 
        left_val = cond.left.int_value;

    if(cond.left.type == EXPR_IDENT) {
        int index = get_variable_index(list, cond.left.ident);
        if(index == -1) {
            printf("Error if: Variable '%s' not found\n",cond.left.ident);
            return 0;
        }

        left_val = list->vars[index].ex.int_value;
    }
    
    int right_val;
    if(cond.right.type == EXPR_INT) 
        right_val = cond.right.int_value;

    if(cond.right.type == EXPR_IDENT) {
        int index = get_variable_index(list,cond.right.ident);
        if(index == -1) {
            printf("Error if: Variable '%s' not found\n",cond.right.ident);
            return 0;
        }

        right_val = list->vars[index].ex.int_value;
    }
    
    switch(cond.type) {
        case COND_EQEQ:
            return left_val == right_val;
        case COND_GE:
            return left_val >= right_val;
        case COND_GT:
            return left_val > right_val;
        case COND_LE:
            return left_val <= right_val;
        case COND_LT:
            return left_val < right_val;
        case COND_NOT_EQ:
            return left_val != right_val;

    }
}

int execute_program(Ast *ast, Variable_list *list) {
    int condition;
    while(ast != NULL) {
        switch(ast->type) {
            case AST_SHOW:
                if(execute_show(ast, list)==-1) return -1;
                break;
            case AST_PUT:
                if(execute_put(ast, list)==-1) return -1;
                break;
            case AST_IF:
                if(execute_eval_cond(list,ast->data.if_stmt.cond)) {
                    execute_program(ast->data.if_stmt.body,list); 
                }
                break;
            case AST_WHILE:
                condition = execute_eval_cond(list,ast->data.while_stmt.cond);
                while(condition != 0) {
                    execute_program(ast->data.while_stmt.body,list);
                }
                break;
        }

        ast = ast->next;
    }

    return 0;
}
