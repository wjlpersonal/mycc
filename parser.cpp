#include "mycc.h"

Node* new_node(Token* t, node_type ty);
Node* new_binary(Token* t, node_type ty, Node* l, Node* r);
Program* parse(Token* tokens);
Function* function(Token**rest, Token* t);

//statement
Node* compound(Token** rest, Token* t);
Node* statement(Token**rest, Token* t);

//expression
Node* expression(Token** rest, Token* t);
Node* logicand(Token**rest, Token* t);
Node* equality(Token**rest, Token* t);
Node* relation(Token**rest, Token* t);
Node* add(Token**rest, Token* t);
Node* term(Token**rest, Token* t);
Node* factor(Token**rest, Token* t);
Node* unary(Token**rest, Token* t);
Node* primary(Token**rest, Token* t);


Node* new_node(Token* t, node_type ty){
    Node* n = new Node();
    *n = Node(t, ty);
    return n;
}

Node* new_assign(Token* t, node_type ty, data_type dty){
	Node* n = new_node(t, ty);
	n->dtype = dty;
	return n;
}
	
Node* new_binary(Token* t, node_type ty, Node* l, Node* r){
    Node* n = new_node(t, ty);
    n->lchild = l;
    n->rchild = r;
    return n;
}


//func =    int identifer () { compound
Function* function(Token** rest, Token* t){
    Function *func = new Function();
	t = skip(t, "int");
    if(t->get_type() != IDENTIFER) err_print("func name is not a IDENTIFER");
    func->name = t->get_str();
	t = t->next();
    t = skip(t, "(");
    t = skip(t, ")");
	*rest = t;
	func->body = compound(rest, t);
    return func;
}


//compound =    statment* }
Node* compound(Token** rest, Token* t){
    Node *s = new_node(t, COMPOUND);
	t = t->next();
	Node **pre = &s;
    while(t->get_str() != "}"){
        if(t->get_type() == EOT) err_print("need }");
		(*pre)->lchild = statement(rest,t);
		pre = &((*pre)->lchild);
		t = *rest;
    }
	t = skip(t, "}");
	*rest = t;
    return s;
}

//statement =   { compound | "return" expr ; | int var ; | expr
Node* statement(Token**rest, Token* t){
    if(t->get_str() == "{")
        return compound(rest, t);
    if(t->get_str() == "return"){
        Node* node = new_node(t, RET);
        Node* expr = expression(rest, t->next());
        node->rchild = expr;
		t = skip(*rest, ";");
		*rest = t;
        return node;
    }
    if(t->get_str() == "int"){
        t = t->next();
        if(t->get_type() != IDENTIFER)
			err_print("int x is not a identififer");
		Node* node = new_assign(t, ASSIGN, INT);
		t = t->next();
		if(t->check("=")){
			t = skip(t, "=");
			node->rchild = expression(rest, t);
			t = *rest;
		}
		t = skip(t, ";");
		*rest = t;	
		return node;
 	}
	Node* s = expression(rest, t);
	*rest = skip(*rest, ";");
	return s;
}

//expr      =   land {|| land}*
Node* expression(Token**rest, Token* t){
	Node *expr = new_node(t, EXPR);
    Node *n = logicand(rest, t);
    t = *rest;
    while(true){
        Token* next = t->next();
        if(t->check("||")){
            n = new_binary(t, OR, n, logicand(&next, next));
            t = next;
            continue;
        }
        *rest = t;
		expr->rchild = n;
        return expr;
    }
}



//land = equality { && quality}*
Node* logicand(Token**rest, Token* t){
    Node *n = equality(rest, t);
    t = *rest;
    while(true){
        Token* next = t->next();
        if(t->check("&&")){
            n = new_binary(t, AND, n, equality(&next, next));
            t = next;
            continue;
        }
        *rest = t;
        return n;
    }
}

//equality  =   relation { [== | !=] relation }*
Node* equality(Token**rest, Token* t){
    Node *n = relation(rest, t);
    t = *rest;
    while(true){
        Token* next = t->next();
        if(t->check("==")){
            n = new_binary(t, EQUAL, n, relation(&next, next));
            t = next;
            continue;
        }
        if(t->check("!=")){
            n = new_binary(t, NE, n, relation(&next, next));
            t = next;
            continue;
        }
        *rest = t;
        return n;
    }
}


//relation  =   add { [<|>|<=|>=] add }*
Node* relation(Token**rest, Token* t){
    Node *n = add(rest, t);
    t = *rest;
    while(true){
        Token* next = t->next();
        if(t->check("<")){
            n = new_binary(t, LT, n, add(&next, next));
            t = next;
            continue;
        }
        if(t->check(">")){
            n = new_binary(t, GT, n, add(&next, next));
            t = next;
            continue;
        }
        if(t->check(">=")){
            n = new_binary(t, GE, n, add(&next, next));
            t = next;
            continue;
        }
        if(t->check("<=")){
            n = new_binary(t, LE, n, add(&next, next));
            t = next;
            continue;
        }
        *rest = t;
        return n;
    }
}



//add       = term { [+-] term }*
Node* add(Token**rest, Token* t){
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
            n = new_binary(t, SUB, n, term(&next, next));
            t = next;
            continue;
        }
        *rest = t;
        return n;
    }
}

//term      = factor {[*/] factor }*
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
	if(t->get_type()==LITNUM) p->val_int = stoi(t->get_str());
    *rest = t->next();
    return p;
}

// prog =   func
Program* parse(Token* tokens){
    Program *prog = new Program();
	tokens = tokens->next();
	prog->functions = function(&tokens, tokens);
   	if(prog == nullptr) err_print( "parsing errror: program");
    if(prog->functions == nullptr) err_print("parsing error: no function");
  
    return prog;
}
 
