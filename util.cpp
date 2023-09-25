#include "mycc.h"

void print_compound(Node* node, int level);

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
    if(!t->check(str)){
		cout << str << ":";
		err_print("skip fail");
    }
    return t->next();
}

string space(int levels){
    string spc =  "    "; 
    string ret = "";
    for(int i=0;i<levels;i++) ret += spc;
    return ret;
}

string idstr(Node* node){
	string num = to_string(node->idcnt);
	if(node->type == VAR)
		return "VAR"+num;
	return node->token->get_str()+"IDNUM FAILED";
}

string print_expr(Node* node){
	if(node == nullptr) return "";
	if(node->type == EXPR) node = node->rchild;
	if(node->type == NUM)
		return node->token->get_str();
	if(node->type == VAR)
		return idstr(node);
	string ret = "";
	Node *l = node->lchild;
	Node *r = node->rchild;
	ret.append("(");
	ret.append(print_expr(l));
	ret.append(node->token->get_str());
	ret.append(print_expr(r));
	if(node->type == CONDITIONAL)
		ret.append(":"+print_expr(node->next));
	ret.append(")");
	return ret;
}

void print_stat(Node* node, int level){
	if(node->type == EXPR) node = node->rchild;
	if(node->type == ASSIGN){
		cout << space(level) << "ASSIGN ";
		cout << (print_expr(node->rchild)) <<endl;
	}
	if(node->type == DECLARE){
		cout<<space(level) <<"DECLARE ";
		cout << idstr(node->lchild) << "=";
		cout << print_expr(node->rchild) << endl;
	}
	if(node->type == RET){
		cout << space(level) << "RET ";
		cout << print_expr(node->rchild) << endl;
	}
	if(node->type == IF){
		cout << space(level) << "IF ";
		cout << print_expr(node->lchild) << ":" << endl;
		if(node->rchild->type == COMPOUND)
			print_compound(node->rchild, level+1);
		else
			print_stat(node->rchild, level+1);
		if(node->next){
			cout << space(level) << "ELSE"<<endl;
			print_stat(node->next, level+1);
		}
	}
}

void print_compound(Node* node, int level){
	if(node->type != COMPOUND) err_print("Printing : not a compound!");
	cout << space(level)<<"{"<<endl;
	node = node->rchild;
	while(node){
		print_stat(node, level+1);
		node = node->next;
	}
	cout <<space(level) <<"}"<<endl;
	return ;
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
		print_compound(body, level);
        level--;
    }
}
