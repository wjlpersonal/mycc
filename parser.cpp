#include "mycc.h"


Node* new_node(Token* t, node_type ty){
	Node* n = new Node();
	*n = Node(t, ty);
	return n;
}

Node* new_binary(Token* t, node_type ty, Node* l, Node* r){
	Node* n = new_node(t, ty);
	n->lchild = l;
	n->rchild = r;
	return n;
}

Node* expression(Token** rest, Token* t);
Function* function(Token* tokens);
Node* term(Token**rest, Token* t);
Node* factor(Token**rest, Token* t);
Node* primary(Token**rest, Token* t);
Node* statement(Token**rest, Token* t);
Node* unary(Token**rest, Token* t);
Node* compound(Token** rest, Token* t);
Program* parse(Token* tokens);

//func =    int identifer (stat*){stat}
Function* function(Token* tokens){
    Function *f = new Function();
    return f;
}

//stat =    return exp;
Node* compound(Token** rest, Token* t){
    Node *s = new Node();
    while(t->get_str() != "}"){
        if(t->get_type() == EOT) err_print("need }");
        while(t->get_str() != ";"){
            if(t->get_type() == EOT) err_print("need ;");
            if(t->check("return")){
                *s = Node(t, RET);
                Node *exp = new Node();
                t = t->next();
                exp = expression(&t, t);
                s->lchild = exp;
                continue;
            }
            t = t->next();
        }
        t = t->next();
    }
    t = t->next();
    *rest = t;
    if(s->lchild == nullptr) err_print("need a return stat");
    return s;
}

//expression		= term +- term
Node* expression(Token**rest, Token* t){
	Node *n = term(rest, t);
	t = *rest;
	while(true){
		Token* next = t->next();
		if(t->check("+")){
			n = new_binary(t, ADD, n, term(&next, next));
			t = next;
			continue;
		}
		if(t->check("-")){
			n = new_binary(t, MUL, n, term(&next, next));
			t = next;
			continue;
		}
		*rest = t;
		return n;
	}
}

//term		= factor */ factor
Node* term(Token**rest, Token* t){
	Node *n = factor(rest, t);
	t = *rest;
	while(true){
		Token* next = t->next();
		if(t->check("*")){
			n = new_binary(t, MUL, n, factor(&next, next));
			t = next;
			continue;
		}
		if(t->check("/")){
			n = new_binary(t, DIV, n, factor(&next, next));
			t = next;
			continue;
		}
		*rest = t;
		return n;
	}
}

//factor     =   op factor | primary 
Node* factor(Token**rest, Token* t){
    if(t->get_type() != PUNCT) 
		return primary(rest, t);
    Node *u = new Node();
    if(t->check("-")){
        *u = Node(t, NEG);
    }
    if(t->check("~")){
        *u = Node(t,BCPL);
    }
    if(t->check("!")){
        *u = Node(t, LNEG);
    }
    t = t->next();
    u->rchild = factor(rest, t);
    if(u->rchild == nullptr) err_print("no unary expr");
    return u;
}

//primary = ( expr ) | num
Node* primary(Token** rest, Token* t){
	if(t->check("(")){
		t = t->next();
		Node* expr = expression(rest, t);
		t = *rest;
		if(! t->check(")")) err_print("need a )");
		*rest = t->next();
		return expr;
	}
	Node *p = new Node();
	p = new_node(t, NUM);
	*rest = t->next();
	return p;
}

// prog =   func
Program* parse(Token* tokens){
    Program *prog = new Program();

    Function *func = new Function();

    Token *t = tokens->next();
    while(t->get_type() != EOT){
        if(!consume(&t, t, "int")){
            t = t->next();
            continue;
        }
        if(t->get_type() != IDENTIFER){
            t = t->next();
            continue;
        }
        func->name = t->get_str();
        t = t->next();
        if(!consume(&t, t, "(")){
            t = t->next();
            continue;
        }
        if(!consume(&t, t, ")")){
            t = t->next();
            continue;
        }
        if(!consume(&t, t, "{")){
            t = t->next();
            continue;
        }
        Node *body = new Node();
        body = compound(&t, t);
        func->body = body;
        prog->functions = func;
    }

    if(prog == nullptr) err_print( "parsing errror: program");
    if(prog->functions == nullptr) err_print("parsing error: no function");
  
    return prog;
}
 
