#ifndef AST_H
#define AST_H

typedef enum {
    AST_SHOW,
    AST_PUT,
    AST_IF,
    AST_WHILE,
    AST_VAR_ASSIGN,
} Ast_type;

typedef enum {
    EXPR_INT,
    EXPR_STR,
    EXPR_IDENT,
    EXPR_READ,
    EXPR_BIN,
} Expr_type;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
} Binop_type;

typedef enum {
    READ_NUM,
    READ_BUFF,
} Read_type;

typedef struct Expr Expr;
typedef struct Expr {
    Expr_type type;
    union {
        int int_value;
        char *str_value;
        char *ident;

        struct {
            Expr *left;
            Binop_type op;
            Expr *right;
        } bin;
        struct {
            char *prompt;
            Read_type type;
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
    Expr *left;
    Cond_type type;
    Expr *right;
} Condition;

typedef struct Ast {
    Ast_type type;
    union {
        struct {
            char *varname;
            Expr *expr;
        } put;
        struct {
            Condition *cond;
            struct Ast *body;
        } if_stmt;
        struct {
            Condition *cond;
            struct Ast *body;
        } while_stmt;
        struct {
            char *varname;
            Expr *ex;
        } var_assign;
        struct {
            Expr *expr;
        } showText;
    } data;
    struct Ast *next;
} Ast;

Expr *expr_new_int(int num);
Expr *expr_new_str(char *str);
Expr *expr_new_ident(char *ident);
Expr *expr_new_read(char *promt, Read_type type);
Expr *expr_new_bin(Expr *left, Binop_type op, Expr *right);
Expr *expr_copy_expr(Expr *ex);
void expr_destroy(Expr *ex);

Condition *cond_new(Expr *left, Cond_type type, Expr *right);
void cond_destroy(Condition *cond);

Ast *ast_new_put(char *varname, Expr *ex);
Ast *ast_new_show(Expr *ex);
Ast *ast_new_if(Condition *cond, Ast *body);
Ast *ast_new_while(Condition *cond, Ast *body);
Ast *ast_new_var_assign(char *varname, Expr *ex);

void ast_append(Ast **head,Ast *node);
void ast_destroy(Ast *ast);

#endif
