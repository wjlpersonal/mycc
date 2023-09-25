#include "mycc.h"

Token* new_token(string str, token_type ty){
	Token *t = new Token();
	*t = Token(str, ty);
	return t;
}

Node* new_node(Token* t, node_type ty);
Node* new_binary(Token* t, node_type ty, Node* l, Node* r);
Program* parse(Token* tokens);
Function* function(Token**rest, Token* t);

//statement
Node* compound(Token** rest, Token* t);
Node* statement(Token**rest, Token* t);

//expression
Node* expression(Token** rest, Token* t);
Node* conditional(Token** rest, Token* t);
Node* logicand(Token**rest, Token* t);
Node* equality(Token**rest, Token* t);
Node* relation(Token**rest, Token* t);
Node* add(Token**rest, Token* t);
Node* term(Token**rest, Token* t);
Node* factor(Token**rest, Token* t);
Node* unary(Token**rest, Token* t);
Node* primary(Token**rest, Token* t);
Node* logicor(Token**rest, Token* t);


Node* new_node(Token* t, node_type ty){
    Node* n = new Node();
    *n = Node(t, ty);
    return n;
}

Node* new_declare(Token* t, node_type ty, data_type dty){
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

static int idcnt = 0;
vector<Node*> lvars;

bool find_var(Token* t){
	string str = t->get_str();
	for(auto var: lvars)
		if(var->token->check(str))
			return true;
	return false;
}

//new a local variable
Node* new_lvar(Token* t){
	if(find_var(t)){
		cout << t->get_str()<<":";
		err_print("redeclaring an identifer!");
	}
	if(t->get_type() != IDENTIFER){
		cout << t->get_str() << " ";
		err_print("is not an IDENTIFER");
	}
	idcnt ++;
	Node *node = new_node(t, VAR);
	node->idcnt = idcnt;
	lvars.push_back(node);
	return node;
}

Node* get_var(Token* t){
	string str = t->get_str();
	for(auto var: lvars){
		if(var->token->check(str))
			return var;
	}
	cout << str << endl;
	err_print("can't find this var");
	return nullptr;
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
		if(*pre == s){
			(*pre)->rchild = statement(rest,t);
			pre = &((*pre)->rchild);
		}
		else{
			(*pre)->next = statement(rest,t);
			pre = &((*pre)->next);
		}
		t = *rest;
    }
	t = skip(t, "}");
	*rest = t;
    return s;
}

//statement =   { compound | if (statement) [else statement]? |
//				"return" expr ; | dtype(int) var ; | expr
Node* statement(Token**rest, Token* t){
    if(t->get_str() == "{")
        return compound(rest, t);
	if(t->get_str() == "if"){
		Node* node = new_node(t, IF);
		t = skip(t->next(), "(");
		node->lchild = expression(rest, t);
		t = skip(*rest, ")");
		if(t->get_str() == "{"){
			t = skip(t, "{");
			node->rchild = compound(rest, t);
		}
		else node->rchild = statement(rest, t);
		t = *rest;
		if(t->get_str() == "else"){
			t = t->next();
			node->next = statement(rest, t);
		}
		if(node->next && node->next->type == DECLARE)
			err_print("can't declare a var in if");
		return node;
	}
	if(t->get_str() == "else")
		err_print("an else without if");
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
		Node* var = new_lvar(t);
		Node* node = new_declare(t, DECLARE, INT);
		node->lchild = var;
		t = t->next();
		if(t->check("=")){
			t = skip(t, "=");
			node->rchild = expression(rest, t);
			t = *rest;
			t = skip(t, ";");
			*rest = t;
			return node;
		}
		Token* zero_t = new_token("0", LITNUM);
		node->rchild = new_node(zero_t, NUM);
		t = skip(t, ";");
		*rest = t;	
		return node;
 	}
	Node* s = new_node(t, EXPR);
	s->rchild = expression(rest, t);
	*rest = skip(*rest, ";");
	return s;
}

//expr 		=	assign   id = expr | conditional
Node* expression(Token**rest, Token* t){
	if(t->get_type() == IDENTIFER && t->next()->check("=")){
		if(!find_var(t)){
			cout << t->get_str()<<":";
			err_print("var not declared");
		}
		Node* var = get_var(t);
		t = t->next();
		Node* expr = expression(rest, t->next());
		return new_binary(t, ASSIGN, var, expr);
	}
	return conditional(rest, t);
}

//conditional = lor [ ? expr : conditional] ? 
Node* conditional(Token**rest , Token* t){
	Node* node = logicor(rest, t);
	t = *rest;
	if(t->get_str() != "?")
		return node;
	node = new_binary(t, CONDITIONAL, node, expression(rest, t->next()));
	t = *rest;
	if(t->get_str() != ":") err_print("need :");
	t = t->next();
	node->next = conditional(rest, t);
	return node;
}

//lor      =   land {|| land}*
Node* logicor(Token**rest, Token* t){
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
        return n;
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
	if(t->get_type()==LITNUM){
    	p = new_node(t, NUM);
		p->val_int = stoi(t->get_str());
	}
	if(t->get_type()==IDENTIFER){
		p = get_var(t);
	}
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
 
