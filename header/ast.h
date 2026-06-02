#ifndef AST_H
#define AST_H

typedef enum {
    AST_SHOW,
    AST_PUT,
} Ast_type;

typedef enum {
    VAL_INT,
    VAL_STR,
    VAL_IDENT,
} Value_type;

typedef struct {
    Value_type type;
    union {
        int int_value;
        char *str_value;
        char *ident;
    } data;
} Value;

typedef struct Ast {
    Ast_type type;
    union {
        struct {
            char *varname;
            Value value;
        } put;
        struct {
            Value value;
        } showText;
    } data;
    struct Ast *next;
} Ast;

Value val_new_int(int num);
Value val_new_str(char *str);
Value val_new_ident(char *ident);
void val_destroy(Value v);

Ast *ast_new_put(char *varname, Value v);
Ast *ast_new_show(Value v);

void ast_append(Ast **head,Ast *node);
void ast_destroy(Ast *ast);

#endif
