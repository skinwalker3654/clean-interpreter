#include "../header/execute.h"
#include <stdio.h>

void execute_put(Ast *ast, Variable_list *list) {
    push_variable_value(list, ast->data.put.varname, ast->data.put.value);
}

void execute_show(Ast *ast, Variable_list *list) {
    int index;
    switch(ast->data.showText.value.type) {
        case VAL_STR: 
            printf("%s\n",ast->data.showText.value.data.str_value);
            break;
        case VAL_IDENT:
            index = get_variable_value(list, ast->data.showText.value.data.ident);
            if(index == -1) {
                printf("Error: Variable %s not found\n",ast->data.put.varname);
                return;
            }

            if(list->vars[index].v.type == VAL_INT)
                printf("%d\n",list->vars[index].v.data.int_value);
            else
                printf("%s\n",list->vars[index].v.data.str_value);
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
