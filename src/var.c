#include "../header/var.h"
#include "../header/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Variable_list *variable_list_init() {
    Variable_list *list = malloc(sizeof(Variable_list));
    if(!list) {
        printf("Failed to create the variable list\n");
        return NULL;
    }

    list->vars = malloc(sizeof(Variable)*2);
    if(!list->vars) {
        printf("Failed to initialize the variable list\n");
        free(list);
        return NULL;
    }
    
    list->counter = 0;
    list->capacity = 2;

    return list;
}

void variable_list_free(Variable_list *list) {
    if(!list) return;

    for(int i=0; i<list->counter; i++) {
        free(list->vars[i].name);
        expr_destroy(list->vars[i].ex);
    }

    free(list->vars);
    free(list);
    list = NULL;
}

int get_variable_value(Variable_list *list, char *name) {
    if(!list) {
        printf("Variable '%s' not found: variable list is empty\n",name);
        return -1;
    }

    int found = -1;
    for(int i=0; i<list->counter; i++) {
        if(strcmp(list->vars[i].name,name)==0) {
            found = i;
            break;
        }
    }

    return found;
}

void push_variable_value(Variable_list *list, char *varname, Expr ex) {
    if(!list) {
        printf("List is not initialized\n");
        return;
    }

    if(list->counter >= list->capacity) {
        list->capacity *= 2;
        Variable *temp = realloc(list->vars, sizeof(Variable) * list->capacity);
        if(!temp) {
            printf("Failed to push the variable into the list\n");
            return;
        }

        list->vars = temp;
    }

    list->vars[list->counter].name = strdup(varname);
    if(!list->vars[list->counter].name) {
        printf("Failed to create variable name\n");
        return;
    }
    
    list->vars[list->counter].ex = expr_copy_expr(ex);
    list->counter++;
}
