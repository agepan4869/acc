#define _GNU_SOURCE

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Type Type;

/**************
 *   tokenize.c
 **************/
// トークンの種類
typedef enum{
    TK_RESERVED,        // 記号
    TK_IDENT,           // 識別子
    TK_NUM,             // 整数トークン
    TK_EOF,             // 入力の終わりを表すトークン
}TokenKind;

// トークン型
typedef struct Token Token;
struct Token{
    TokenKind kind;     // トークンの型
    Token *next;        // 次の入力トークン
    long val;            // kindがTK_NUMの場合、その数値
    char *str;          // トークン文字列
    int len;            // トークンの長さ
};

void error(char *fmt, ...);
void error_at(char *loc,char *fmt, ...);
void error_tok(Token *tok,char *fmt, ...);
Token *consume(char *op);
Token *consume_ident();
void expect(char *op);
long expect_number();
char *expect_ident();
bool at_eof();
Token *tokenize();

extern char *user_input;
extern Token *token;

/*************** 
 *   parse.c
 ***************/
typedef struct Var Var;
struct Var{
    char *name;
    int offset;
};

typedef struct VarList VarList;
struct VarList{
    VarList *next;
    Var *var;
};

typedef enum{
    ND_ADD,         // num + num
    ND_PTR_ADD,     // ptr + num or num + ptr
    ND_SUB,         // num - num
    ND_PTR_SUB,     // ptr - num
    ND_PTR_DIFF,    // ptr - ptr
    ND_MUL,         // *
    ND_DIV,         // /
    ND_EQ,          // ==
    ND_NE,          // !=
    ND_LT,          // <,>
    ND_LE,          // <=,>=
    ND_ASSIGN,      // =
    ND_ADDR,        // unary &
    ND_DEREF,       // unary *
    ND_NUM,         // 整数
    ND_RETURN,      // return
    ND_IF,          // if
    ND_WHILE,       // while
    ND_FOR,         // for
    ND_BLOCK,       // {...}
    ND_FUNCALL,     // 関数呼び出し
    ND_VAR,         // 変数
    ND_EXPR_STMT    // 式のステートメント
}NodeKind;

typedef struct Node Node;
struct Node{
    NodeKind kind;      // ノードの型
    Node *next;         // 次のノード
    Type *ty;           // タイプ eg: int or pointer to int
    Token *tok;         // 代表トークン
    Node *lhs;          // 左辺
    Node *rhs;          // 右辺

    // if,while,forステートメント
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;

    // Block
    Node *body;

    // Function call
    char *funcname;
    Node *args;

    Var *var;          // kindがND_VARの場合のみ使う
    long val;           // kindがND_NUMの場合のみ使う
};

typedef struct Function Function;
struct Function{
    Function *next;
    char *name;
    VarList *params;
    Node *node;
    VarList *locals;
    int stack_size;
};

Function *program();

/*****************
 *  typing.c
 *****************/

typedef enum{TY_INT,TY_PTR}TypeKind;

struct Type{
    TypeKind kind;
    Type *base;
};

bool is_integer(Type *ty);
void add_type(Node *node);

/*****************
*   codegen.c
******************/
void codegen(Function *prog);