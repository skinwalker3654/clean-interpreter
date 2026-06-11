#ifndef EXECUTE_H
#define EXECUTE_H

#include "ast.h"
#include "var.h"

int execute_put(Ast *ast,Variable_list *list);
int execute_show(Ast *ast,Variable_list *list);
int execute_program(Ast *ast,Variable_list *list);

#endif
