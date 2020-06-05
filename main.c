#include "acc.h"

int main(int argc,char **argv){
    if(argc != 2)
        error("%s: 引数の個数が正しくありません",argv[0]);

    // トークナイズしてパースする
    // 結果はcodeに保存される
    user_input = argv[1];
    token = tokenize();
    Function *prog = program();

    for(Function *fn = prog; fn; fn=fn->next){
        int offset = 0;

        for(Var *var=prog->locals; var; var=var->next){
            offset += 8;
            var->offset = offset;
        }
        fn->stack_size = offset;
    }
    codegen(prog);
    return 0;
}