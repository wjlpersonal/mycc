#include "mycc.h"

int clausecnt = 0;

string re_string(string str, string snip){
	string ret = regex_replace(str, regex("%s"), snip);
	return ret;
}

bool node_check(Node* node, vector<node_type>& list){
	for(auto ty : list)
		if(node->type == ty) return true;
	return false;
}

string get_clause(){
	clausecnt ++;
	string ret = "_clase%s";
	ret = re_string(ret, to_string(clausecnt));
	return ret;
}

string gen_expr(Node* node){
	if(node == nullptr) return "";
	if(node->type == EXPR)
		node = node->rchild;
	string ret = "";
	if(node->type == VAR){

	}
	if(node->type == NUM){
		string num = node->token->get_str();
		ret.append(re_string("movl   $%s, \%eax\n", num));
	}
	if(node->type == NEG || node->type == BCPL){
		ret.append("neg    \%eax\n");
	}
	if(node->type == LNEG){
		ret.append("cmpl   $0, \%eax\n");
		ret.append("movl   $0, \%eax\n");
		ret.append("sete   \%al\n");
	}
	if(node->type == ADD){
		ret.append(gen_expr(node->rchild));
		ret.append("push   \%eax\n");
		ret.append(gen_expr(node->lchild));
		ret.append("pop    \%ebx\n");
		ret.append("add    \%ebx, \%eax\n");
	}
	if(node->type == SUB){
		ret.append(gen_expr(node->rchild));
		ret.append("push   \%eax\n");
		ret.append(gen_expr(node->lchild));
		ret.append("pop    \%ebx\n");
		ret.append("sub    \%ebx, \%eax\n");
	}
	if(node->type == MUL){
		ret.append(gen_expr(node->rchild));
		ret.append("push   \%eax\n");
		ret.append(gen_expr(node->lchild));
		ret.append("pop    \%ebx\n");
		ret.append("mul    \%ebx, \%eax\n");
	}
	if(node->type == DIV){
		ret.append(gen_expr(node->rchild));
		ret.append("push   \%eax\n");
		ret.append(gen_expr(node->lchild));
		ret.append("pop    \%ebx\n");
		ret.append("div    \%ebx, \%eax\n");
	}
	if(node->type == LT){
		ret.append(gen_expr(node->rchild));
		ret.append("push   \%eax\n");
		ret.append(gen_expr(node->lchild));
		ret.append("pop    \%ebx\n");
		ret.append("cmpl   \%ebx, \%eax\n");
		ret.append("movl   $0, \%eax\n");
		ret.append("setlt  \%al\n");
	}
	if(node->type == LE){
		ret.append(gen_expr(node->rchild));
		ret.append("push   \%eax\n");
		ret.append(gen_expr(node->lchild));
		ret.append("pop    \%ebx\n");
		ret.append("cmpl   \%ebx, \%eax\n");
		ret.append("movl   $0, \%eax\n");
		ret.append("setle  \%al\n");
	}
	if(node->type == GT){
		ret.append(gen_expr(node->rchild));
		ret.append("push   \%eax\n");
		ret.append(gen_expr(node->lchild));
		ret.append("pop    \%ebx\n");
		ret.append("cmpl   \%ebx, \%eax\n");
		ret.append("movl   $0, \%eax\n");
		ret.append("setgt  \%al\n");
	}
	if(node->type == GE){
		ret.append(gen_expr(node->rchild));
		ret.append("push   \%eax\n");
		ret.append(gen_expr(node->lchild));
		ret.append("pop    \%ebx\n");
		ret.append("cmpl   \%ebx, \%eax\n");
		ret.append("movl   $0, \%eax\n");
		ret.append("setge  \%al\n");
	}
	if(node->type == EQUAL){
		ret.append(gen_expr(node->rchild));
		ret.append("push   \%eax\n");
		ret.append(gen_expr(node->lchild));
		ret.append("pop    \%ebx\n");
		ret.append("cmpl   \%ebx, \%eax\n");
		ret.append("movl   $0, \%eax\n");
		ret.append("sete   \%al\n");
	}
	if(node->type == NE){
		ret.append(gen_expr(node->rchild));
		ret.append("push   \%eax\n");
		ret.append(gen_expr(node->lchild));
		ret.append("pop    \%ebx\n");
		ret.append("cmpl   \%ebx, \%eax\n");
		ret.append("movl   $0, \%eax\n");
		ret.append("setne  \%al\n");
	}
	if(node->type == AND){
		ret.append(gen_expr(node->lchild));
		ret.append("cmpl   $0, \%eax\n");
		string clause_a = get_clause();
		string clause_b = get_clause();
		ret.append("je     "+clause_a+"\n");
		ret.append(gen_expr(node->rchild));
		ret.append("cmpl   $0, \%eax\n");
		ret.append("je     "+clause_a+"\n");
		ret.append("movl   $1, \%eax\n");
		ret.append("jmp    "+clause_b+"\n");
		ret.append(clause_a+":\n");
		ret.append("movl   $0, \%eax\n");
		ret.append(clause_b+":\n");
	}
	if(node->type == OR){
		ret.append(gen_expr(node->lchild));
		ret.append("cmpl   $1, \%eax\n");
		string clause_a = get_clause();
		string clause_b = get_clause();
		ret.append("je     "+clause_a+"\n");
		ret.append(gen_expr(node->rchild));
		ret.append("cmpl   $1, \%eax\n");
		ret.append("je     "+clause_a+"\n");
		ret.append("movl   $0, \%eax\n");
		ret.append("jmp    "+clause_b+"\n");
		ret.append(clause_a+":\n");
		ret.append("movl   $1, \%eax\n");
		ret.append(clause_b+":\n");
	}
	if(node->type == ASSIGN){


	}
	return ret;
}


string gen_stat(Node* node){
	string ret = gen_expr(node->rchild);
	if(node->type == RET){
		ret.append("ret\n");
	}
	if(node->type == COMPOUND){
		node = node->lchild;
		ret.append(gen_stat(node));
	}
	return ret;
}

string gen_compound(Node* node){
	if(!node) return "";
	node = node->rchild;
	string ret = "";
	while(node){
		ret.append(gen_stat(node));
		node = node->next;
	}
	return ret;
}

string codegen(Program* prog){
    string ret = ".global %s\n_%s:\n";
    Function *func = prog->functions;
    ret = re_string(ret, func->name);
    ret.append(gen_compound(func->body));
    return ret;
}
