#include "mycc.h"

void err_print(string str){
    cout << str << endl;
    exit(1);
}

bool is_number(const std::string& s){
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

bool consume(Token **rest, Token *t, string str){
    if(t->get_str() == str){
        *rest = t->next();
        return true;
    }
    *rest = t;
    return false;
}

Token* skip(Token *t, string str){
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

string print_expr(Node* node){
	if(node == nullptr) return "";
	if(node->type == NUM)
		return node->token->get_str();
	string ret = "";
	Node *l = node->lchild;
	Node *r = node->rchild;
	ret.append("(");
	ret.append(print_expr(l));
	ret.append(node->token->get_str());
	ret.append(print_expr(r));
	ret.append(")");
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
			cout << print_expr(exp);
            cout << endl;
        }
        level--;
    }
}
