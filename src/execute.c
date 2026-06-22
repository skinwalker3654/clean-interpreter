#include "../header/execute.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int execute_eval_bin(Variable_list *list, Expr *ex) {
    int lhs;
    int rhs;

    int index;
    switch(ex->type) {
        case EXPR_INT:
            return ex->int_value;
        case EXPR_IDENT:
            index = get_variable_index(list,ex->ident);
            if(index == -1) {
                printf("Error: Variable '%s' not found\n",ex->ident);
                return -1;
            }

            if(list->vars[index].ex->type == EXPR_STR) {
                printf("Error: Variable contains a string\n");
                return -1;
            }

            return list->vars[index].ex->int_value;
        case EXPR_BIN: {
            lhs = execute_eval_bin(list,ex->bin.left);
            rhs = execute_eval_bin(list,ex->bin.right);

            switch(ex->bin.op) {
                case OP_ADD:
                    return lhs + rhs;
                case OP_SUB:
                    return lhs - rhs;
                case OP_MUL:
                    return lhs * rhs;
                case OP_DIV:
                    return lhs / rhs;
            }
        } 
        default:
            break;
    }

    printf("Error: Invalid expression\n");
    return -1;
}

int execute_put(Ast *ast, Variable_list *list) {
    char *buffer = NULL;
    size_t size = 0;
    Expr *ex;

    switch(ast->data.put.expr->type) {
        case EXPR_READ:
            printf("%s",ast->data.put.expr->read_var.prompt);
            getline(&buffer,&size,stdin);
            buffer[strcspn(buffer,"\n")] = '\0';

            Expr *new_expr = expr_new_str(buffer);
            push_variable_value(list, ast->data.put.varname, new_expr);
            expr_destroy(new_expr);
            free(buffer);
            return 0;

        case EXPR_IDENT:
        case EXPR_INT:
        case EXPR_STR:
            push_variable_value(list, ast->data.put.varname, ast->data.put.expr);
            return 0;

        case EXPR_BIN:
            ex = expr_new_int(execute_eval_bin(list, ast->data.put.expr));
            push_variable_value(list, ast->data.put.varname, ex);
            expr_destroy(ex);
            return 0;
    }
}

int execute_show(Ast *ast, Variable_list *list) {
    int index;
    int result;
    switch(ast->data.showText.expr->type) {
        case EXPR_STR: 
            printf("%s\n",ast->data.showText.expr->str_value);
            return 0;
        case EXPR_IDENT:
            index = get_variable_index(list, ast->data.showText.expr->ident);
            if(index == -1) {
                printf("Error showText: Variable '%s' not found\n",ast->data.showText.expr->ident);
                return -1;
            }

            if(list->vars[index].ex->type == EXPR_INT)
                printf("%d\n",list->vars[index].ex->int_value);
            else
                printf("%s\n",list->vars[index].ex->str_value);
            return 0;
        case EXPR_BIN:
             result = execute_eval_bin(list,ast->data.showText.expr);
             printf("%d\n",result);
             return 0;
        case EXPR_INT:
             printf("%d\n",ast->data.showText.expr->int_value);
             return 0;
        default:
            return -1;
    }
}

int execute_var_assign(Ast *ast, Variable_list *list) {
    Expr *ex;
    switch(ast->data.var_assign.ex->type) {
        case EXPR_STR:
            return set_variable_value(list, ast->data.var_assign.varname,ast->data.var_assign.ex);
        default:
            ex = expr_new_int(execute_eval_bin(list,ast->data.var_assign.ex));
            if(!ex) return -1;

            set_variable_value(list,ast->data.var_assign.varname,ex);
            expr_destroy(ex);

            return 0;
    }
}

static int eval_variable(Expr *var) {
    if(var->type != EXPR_IDENT && var->type != EXPR_INT) {
        printf("Error: Value on the if statment has an invalid value");
        return 0;
    }

    return  1;
}

int execute_eval_cond(Variable_list *list, Condition *cond) {
    if(!eval_variable(cond->left)) return 0;
    if(!eval_variable(cond->right)) return 0;

    int left_val;
    if(cond->left->type == EXPR_INT) 
        left_val = cond->left->int_value;

    if(cond->left->type == EXPR_IDENT) {
        int index = get_variable_index(list, cond->left->ident);
        if(index == -1) {
            printf("Error if: Variable '%s' not found\n",cond->left->ident);
            return 0;
        }

        left_val = list->vars[index].ex->int_value;
    }
    
    int right_val;
    if(cond->right->type == EXPR_INT) 
        right_val = cond->right->int_value;

    if(cond->right->type == EXPR_IDENT) {
        int index = get_variable_index(list,cond->right->ident);
        if(index == -1) {
            printf("Error if: Variable '%s' not found\n",cond->right->ident);
            return 0;
        }

        right_val = list->vars[index].ex->int_value;
    }
    
    switch(cond->type) {
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
                while(execute_eval_cond(list,ast->data.while_stmt.cond)) {
                    execute_program(ast->data.while_stmt.body,list);
                }
                break;
            case AST_VAR_ASSIGN:
                if(execute_var_assign(ast,list)==-1) return -1;
                break;
        }

        ast = ast->next;
    }

    return 0;
}
