#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <regex>
using namespace std;

class Token;

enum token_type{
    IDENTIFER,
    KEY,
    LITERAL,
    PUNC,
    EOT//End of tokens
};

enum node_type{
    FIL,
    KINT,
    NUMINT, 
    DIV,
    SEMI,
    RET,
    FUN,
    VAR,
    LBRACE,
    RBRACE,
    LPAREN,
    RPAREN,
    LBOX,
    RBOX,
    

};

enum child_order{
    LCHILD,
    RCHILD
};

class Node{
    public:
        node_type type;
        string str;
        Node *parent;
        Node *lchild;
        Node *rchild;

        int  val_int;
        

        Node(){}
        Node(string str, node_type type){
            //this->parent = parent;
            this->str = str;
            this->type = type;
        }
        void child(Node* c){
            if(lchild->str.size() != 0){
                rchild = c;
            }
            else    lchild = c;
        }
};

class Expression{
    public:
        int val;
        
};

class Statement{
    public:
        string str;
        Expression* exps;
        
};

class Function{
    public:
        Statement *statements;
        string name;
};

class Program{
    public:
        Function *functions;
};

class Token{
    private:
        string str="";
        token_type type=KEY;
        Token* next_token;
        
    public:
        Token(){}
        Token(string str, token_type type){
            this->str = str;
            this->type = type;
        }
        void show(){cout << str << " : " << type << endl;} 
        token_type get_type(){return type;}
        string get_str(){return str;}
        void set_next(Token* t){
            next_token = t;
        }
        Token* next(){return next_token;}
        static bool check(Token *t, string str){
            if(t->get_str() == str){
                t = t->next();
                return true;
            }
            return false;
        }
 
        static bool check_type(Token *t, token_type type){
            if(t->get_type() == type){
                t = t->next();
                return true;
            }
            return false;
        }
};


//func =    int identifer (stat*){stat}
Function* function(Token* tokens){
    Function *f = new Function();
    return f;
}

//stat =    return exp;
Statement* statement(Token* tokens){
    Statement *s = new Statement();
    Token *t = tokens;
    while(t->get_str() != "}"){
        while(t->get_str() != ";"){
            if(Token::check(t, "return")){
                s->str = "return";
                Expression exp;
                exp.val = stoi(t->get_str());
                t = t->next();
                s->exps = &exp;
            }
        }
        t = t->next();
    }
    t = t->next();
    return s;
}

// prog =   func
Program* parse(Token* tokens){
    Program *prog = new Program();

    Function func;
    Statement stat;

    Token *t = tokens->next();
    while(t->get_type() != EOT){
        Function *func = new Function();

        if(!Token::check(t, "int")){
            t = t->next();
            continue;
        }
        if(t->get_type() != IDENTIFER){
            t = t->next();
            continue;
        }
        func->name = t->get_str();
        if(!Token::check(t, "(")){
            t = t->next();
            continue;
        }
        if(!Token::check(t, ")")){
            t = t->next();
            continue;
        }
        if(!Token::check(t, "{")){
            t = t->next();
            continue;
        }
        Statement *stat = statement(t);
        func->statements = stat;
        prog->functions = func;
    }

    return prog;
}
   
   
void space(int levels){
    for(int i=0;i<levels;i++) cout << "    " ;
}
void Pretty_printing(Program* prog){
    //cout << "PROGRAM " << endl;
    Function *func = prog->functions;
    int level = 1;
    while(func != nullptr){
        cout << "FUN INT "<< func->name << ":"<<endl;
        space(1);
        cout << "params:"<<endl;
        space(1);
        cout <<"body:"<<endl;
        Statement* stat = func->statements;
        level++;
        while(stat != nullptr){
            Expression *exp = stat->exps;
            space(level);
            cout << "RETURN " << exp->val << endl;
        }
        level--;
    }
        
}

int main(void){
    cout << "begin" << endl;
    ifstream infile;
    infile.open(".\\test\\test.c",ios::in);
    if(!infile.is_open()){
        cout << "false"<<endl;
        return 0;
    }
    string line;
    vector<string> buff;
    while(getline(infile,line)){
        buff.push_back(line);
    }
    
    Token *tokens = new Token();
    Token *pre = tokens;
    Token test = Token("test",EOT);
    regex re_op("[\\+\\-\\*\\/]");
    regex re_num("[0-9]+");
    regex re_keyword("int|return|float");
    regex re_key("[\\(\\)\\{\\}\\[\\]\\;\\+\\-\\*\\/]");
    regex re("[\\(\\)\\{\\}\\[\\]\\;]|[^\\(\\)\\{\\}\\[\\]\\;\\s]+");
    for (auto line : buff){
        regex_iterator<string::iterator> reg_it(line.begin(), line.end(), re);
        regex_iterator<string::iterator> rend;
        while(reg_it != rend){
            string str = reg_it->str();
            Token *token = new Token();
            if(regex_match(str, re_key) || regex_match(str, re_keyword)){
                *token = Token(str, KEY);
            }
            else if(regex_match(str, re_num)){
                *token = Token(str, LITERAL);
            }
            else{
                *token = Token(str, IDENTIFER);
            }
            pre->set_next(token);
            pre = token; 

            reg_it++;
        }
    }
    Token t_end("", EOT);
    pre->set_next(&t_end);
    infile.close();

    Token *t = tokens->next();
    while (t->get_type()!= EOT){
        t->show();
        t = t->next();
    }
    Program *prog = parse(tokens); 
    Pretty_printing(prog);
    
    // cout << "over" << endl;

    
    return 0;
}