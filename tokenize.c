#include "acc.h"

// 入力プログラム
char *user_input;

// 現在着目しているトークン
Token *token;

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...){
    va_list ap;
    va_start(ap,fmt);
    vfprintf(stderr,fmt,ap);
    fprintf(stderr,"\n");
    exit(1);
}

// エラー箇所を報告する
static void verror_at(char *loc,char *fmt, va_list ap){

    int pos = loc - user_input;
    fprintf(stderr,"%s\n",user_input);
    fprintf(stderr,"%*s",pos,"");       // posこの空白を出力
    fprintf(stderr,"^ ");
    vfprintf(stderr,fmt,ap);
    fprintf(stderr,"\n");
    exit(1);
}

void error_at(char *loc,char *fmt, ...){
    va_list ap;
    va_start(ap,fmt);
    verror_at(loc, fmt, ap);
}

void error_tok(Token *tok, char *fmt, ...){
    va_list ap;
    va_start(ap,fmt);
    verror_at(tok->str, fmt, ap);
}

// 次のトークンが期待している記号のときには、トークンを1つを読み進めて
// 真を返す。それ以外の場合には偽を返す。
Token *consume(char *op){
    if(token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str,op,token->len))
        return NULL;
    Token *t = token;
    token = token->next;
    return t;
}

Token *peek(char *s){
    if(token->kind != TK_RESERVED || strlen(s) != token->len || strncmp(token->str, s, token->len))
        return NULL;
    return token;
}

Token *consume_ident(){
    if(token->kind != TK_IDENT)
        return NULL;
    Token *t = token;
    token = token->next;
    return t;
}


// 次のトークンが期待している記号のときには、トークンを一つ読み進める。
// それ以外の場合にはエラーを報告する
void expect(char *s){
    if(!peek(s))
        error_tok(token, "expected \"%s\"",s);
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
long expect_number(){
    if(token->kind != TK_NUM)
        error_tok(token,"数ではありません");
    long val = token->val;
    token = token->next;
    return val;
}

char *expect_ident(){
    if(token->kind != TK_IDENT)
        error_tok(token,"expected an identifier");
    char *s = strndup(token->str,token->len);
    token = token->next;
    return s;
}

bool at_eof(){
    return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurにつなげる
static Token *new_token(TokenKind kind,Token *cur,char *str,int len){
    Token *tok = calloc(1,sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

static bool startswith(char *p,char *q){
    return memcmp(p,q,strlen(q)) == 0;
}

static bool is_alpha(char c){
    return('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

static bool is_alnum(char c){
    return is_alpha(c) || ('0' <= c && c <= '9');
}

static char *starts_with_reserved(char *p){
    // keyword
    static char *kw[] = {"return","if","else","while","for","int", "sizeof"};

    for(int i=0; i<sizeof(kw)/sizeof(*kw); i++){
        int len = strlen(kw[i]);
        if(startswith(p,kw[i]) && !is_alnum(p[len]))
            return kw[i];
    }

    // multi-letter punctuator
    static char *ops[] = {"==","!=","<=",">="};

    for(int i=0; i<sizeof(ops)/sizeof(*ops); i++)
        if(startswith(p,ops[i]))
            return ops[i];

    return NULL;
}

// 入力文字列pをトークないずしてそれを返す
Token *tokenize(){
    char *p = user_input;
    Token head = {};
    Token *cur = &head;

    while(*p){
        // 空白文字をスキップ
        if(isspace(*p)){
            p++;
            continue;
        }

        // キーワード
        char *kw = starts_with_reserved(p);

        if(kw){
            int len = strlen(kw);
            cur = new_token(TK_RESERVED,cur,p,len);
            p += len;
            continue;
        }

        if(is_alpha(*p)){
            char *q = p++;
            while(is_alnum(*p))
                p++;
            cur = new_token(TK_IDENT,cur,q,p - q);
            continue;
        }

        if(ispunct(*p)){
            cur = new_token(TK_RESERVED,cur,p++,1);
            continue;
        }

        if(isdigit(*p)){
            cur = new_token(TK_NUM,cur,p,0);
            char *q = p;
            cur->val = strtol(p,&p,10);
            cur->len = p - q;
            continue;
        }

        error_at(p,"トークナイズできません");
    }

    new_token(TK_EOF,cur,p,0);
    return head.next;
}