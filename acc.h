#define _GNU_SOURCE

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
bool consume(char *op);
Token *consume_ident();
void expect(char *op);
long expect_number();
bool at_eof();
Token *tokenize();

extern char *user_input;
extern Token *token;

/*************** 
 *   parse.c
 ***************/
typedef struct Var Var;
struct Var{
    Var *next;
    char *name;
    int offset;
};

typedef enum{
    ND_ADD,         // +
    ND_SUB,         // -
    ND_MUL,         // *
    ND_DIV,         // /
    ND_EQ,          // ==
    ND_NE,          // !=
    ND_LT,          // <,>
    ND_LE,          // <=,>=
    ND_ASSIGN,      // =
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

    Var *var;          // kindがND_VARの場合のみ使う
    long val;           // kindがND_NUMの場合のみ使う
};

typedef struct Function Function;
struct Function{
    Node *node;
    Var *locals;
    int stack_size;
};

Function *program();

/*****************
*   codegen.c
******************/
void codegen(Function *prog);