#ifndef AST_H
#define AST_H

typedef enum {
    AST_SHOW,
    AST_PUT,
} Ast_type;

typedef enum {
    EXPR_INT,
    EXPR_STR,
    EXPR_IDENT,
    EXPR_READ,
} Expr_type;

typedef struct {
    Expr_type type;
    union {
        int int_value;
        char *str_value;
        char *ident;

        struct {
            char *prompt;
        } read_var;
    };
} Expr;

typedef struct Ast {
    Ast_type type;
    union {
        struct {
            char *varname;
            Expr expr;
        } put;
        struct {
            Expr expr;
        } showText;
    } data;
    struct Ast *next;
} Ast;

Expr expr_new_int(int num);
Expr expr_new_str(char *str);
Expr expr_new_ident(char *ident);
Expr expr_new_read(char *prompt);
Expr expr_copy_expr(Expr ex);
void expr_destroy(Expr v);

Ast *ast_new_put(char *varname, Expr ex);
Ast *ast_new_show(Expr ex);

void ast_append(Ast **head,Ast *node);
void ast_destroy(Ast *ast);

#endif
