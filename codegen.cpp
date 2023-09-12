#include <mycc.h>

string gen_ret(Node* stat){
    Node* exp = stat->lchild;
    string num = to_string(exp->val_int);
    string cmd = regex_replace("movl    $\%d, \%eax", regex("\%d"), num);
    string ret = "";
    ret += space(1)+cmd + "\n";
    cmd = "ret";
    ret += space(1)+cmd;
    return ret;
}
string codegen(Program* prog){
    string ret = "";
    string fun= ".global _$\n_$:";
    Function *func = prog->functions;
    string name = func->name;
    fun = regex_replace(fun, regex("[$]"), name);
    ret.append(fun + "\n");
    ret.append(gen_ret(func->body));
    ret.append("\n");
    return ret;
    
}