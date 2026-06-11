#ifndef AST_H
#define AST_H

typedef enum {
    AST_SHOW,
    AST_PUT,
    AST_IF,
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

typedef enum {
    COND_EQEQ,
    COND_GE,
    COND_GT,
    COND_LE,
    COND_LT,
    COND_NOT_EQ,
} Cond_type;

typedef struct {
    Cond_type type;
    Expr left;
    Expr right;
} Condition;

typedef struct Ast {
    Ast_type type;
    union {
        struct {
            char *varname;
            Expr expr;
        } put;
        struct {
            Condition cond;
            struct Ast *body;
        } if_stmt;
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

Condition cond_new(Expr left, Cond_type type, Expr right);
void cond_destroy(Condition cond);

Ast *ast_new_put(char *varname, Expr ex);
Ast *ast_new_show(Expr ex);
Ast *ast_new_if(Condition cond, Ast *body);

void ast_append(Ast **head,Ast *node);
void ast_destroy(Ast *ast);

#endif
