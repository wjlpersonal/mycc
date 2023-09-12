#include <mycc.h>

//unary     =   op exp
Node* unary(Token**rest, Token* t){
    Node *u = new Node();
    Node *exp;
    if(t->get_type() != PUNCT) err_print("not a unary");
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
    exp = expression(rest, t);
    u->lchild = exp;
    if(u->lchild == nullptr) err_print("no unary expr");
    return u;
}

//func =    int identifer (stat*){stat}
Function* function(Token* tokens){
    Function *f = new Function();
    return f;
}

//expr      =   primary | unary
Node* expression(Token** rest, Token* t){
    Node *exp;
    if(t->get_type() == LITNUM){
        exp = new Node();
        *exp = Node(t, NUM);
        exp->val_int = stoi(t->get_str().c_str());
        *rest = t->next();
        return exp;
    }
    if(t->get_type() == PUNCT){
        exp = unary(rest, t);
        return exp;
    }
    err_print("not valid expr");
    return nullptr;
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

    return prog;
}
 