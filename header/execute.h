#ifndef EXECUTE_H
#define EXECUTE_H

#include "ast.h"
#include "var.h"

void execute_put(Ast *ast,Variable_list *list);
void execute_show(Ast *ast,Variable_list *list);
void execute_program(Ast *ast,Variable_list *list);

#endif
