#ifndef EXECUTE_H
#define EXECUTE_H

#include "ast.h"
#include "var.h"

int execute_put(Ast *ast,Variable_list *list);
int execute_show(Ast *ast,Variable_list *list);
int execute_var_assign(Ast *ast,Variable_list *list);
int execute_eval_cond(Variable_list *list,Condition *cond);
int execute_eval_bin(Variable_list *list,Expr *ex);
int execute_program(Ast *ast,Variable_list *list);

#endif
