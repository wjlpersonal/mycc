#include <mycc.h>

void err_print(string str){
    cout << str << endl;
    exit(1);
}

bool is_number(const std::string& s){
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

static bool consume(Token **rest, Token *t, string str){
    if(t->get_str()== str){
        *rest = t->next();
        return true;
    }
    *rest = t;
    return false;
}

static Token* skip(Token *t, string str){
    if(t->check(str)){
        return t->next();
    }
    return t;
}

string space(int levels){
    string spc =  "    "; 
    string ret = "";
    for(int i=0;i<levels;i++) ret += spc;
    return ret;
}

void Pretty_printing(Program* prog){
    Function *func = prog->functions;
    int level = 1;
    if(func != nullptr){
        cout << "FUN INT "<< func->name << ":"<<endl;
        cout<<space(level) << "params:"<<endl;
        cout <<space(level) <<"body:"<<endl;
        Node* body = func->body;
        level++;
        if(body != nullptr){
            Node* exp = body->lchild;
            cout <<space(level) << "RETURN ";
            while(exp != nullptr){
                cout << exp->token->get_str();
                exp = exp->lchild;
            }
            cout << endl;
        }
        level--;
    }
}