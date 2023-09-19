#include "mycc.h"

vector<node_type> unary = {NEG, LNEG, BCPL};
vector<node_type> binary = {ADD, SUB, MUL, DIV};

bool node_check(Node* node, vector<node_type>& list){
	for(auto ty : list)
		if(node->type == ty) return true;
	return false;
}

string gen_expr(Node* node){
	string ret = "";
	if(node_check(node, unary)){
		

}

//term      = factor {[*/] factor }*
string gen_term(Node* node){
	string ret = gen_factor(node->lchild);
	if(node->type == MUL){
		


//factor = op factor | primary
string gen_factor(Node* node){
	if(!node_check(node, unary))
		return gen_primary(node);
	string ret = gen_factor(node->rchild);
	if(node->type == NEG || node->type == BCPL){
		ret.append("neg    \%eax\n");
	}
	if(node->type == LNEG){
		reg.append("cmpl   $0, \%eax\n");
		reg.append("movl   $0, \%eax\n");
		reg.append("sete   \%al\n");
	}
	return ret;
}

//primary = (expr) | num
string gen_primary(Node* node){
	string ret = "";
	if(node->type == NUM){
		string num = node->token->get_str();
		ret.append(re_string("movl   $\\s, \%eax\n", num));
	}
	if(node->type == EXPR){
		string expr = gen_expr(node->rchild);
		ret.append(expr);
	}
	return ret;
}

string gen_ret(Node* node){
	string ret = "";
	string expr = gen_expr(node->rchild);
	ret.append(expr);
	ret.append("ret\n");
	return ret;
}

string re_string(string str, string snip){
	string ret = regex_replace(str, regex("\\s"), snip);
	return ret;
}

string codegen(Program* prog){
    string ret = "";
    string fun= ".global \\s\n_\\s:\n";
    Function *func = prog->functions;
    fun = re_string(fun, func->name);
    ret.append(gen_expr(func->body));
    return ret;
}
