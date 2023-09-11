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
        Expression* exps = nullptr;
        
};

class Function{
    public:
        Statement *statements = nullptr;
        string name;
};

class Program{
    public:
        Function *functions = nullptr;
};

class Token{
    private:
        string str="";
        token_type type=KEY;
        Token* next_token = nullptr;
        
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
            return t->get_str() == str;
        } 

        static bool check_type(Token *t, token_type type){
            if(t->get_type() == type){
                t = t->next();
                return true;
            }
            return false;
        }
};
static bool consume(Token **rest, Token *t, string str){
    if(t->get_str()== str){
        *rest = t->next();
        return true;
    }
    *rest = t;
    return false;
}

static Token* skip(Token *t, string str){
    if(Token::check(t, str)){
        return t->next();
    }
    return t;
}
 

bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

//func =    int identifer (stat*){stat}
Function* function(Token* tokens){
    Function *f = new Function();
    return f;
}

//stat =    return exp;
Statement* statement(Token** rest, Token* tokens){
    Statement *s = new Statement();
    Token *t = tokens;
    while(t->get_str() != "}"){
        if(t->get_type() == EOT){
            cout << "need }" << endl;
            exit(1);
        }
        while(t->get_str() != ";"){
            if(t->get_type() == EOT){
                cout << "need ;" << endl;
                exit(1);
            }
            if(Token::check(t, "return")){
                s->str = "return";
                Expression *exp = new Expression();
                t = t->next();
                if(t->get_type() != LITERAL){
                    cout << "return value wrong "<< endl;
                    exit(1);
                }
                if(!is_number(t->get_str())){
                    cout << "return value is not a number "<<endl;
                    exit(1);
                }
                exp->val = stoi(t->get_str());
                t = t->next();
                s->exps = exp;
                continue;
            }
            t = t->next();
        }
        t = t->next();
    }
    t = t->next();
    *rest = t;
    if(s->exps == nullptr){
        cout << "no return statement" << endl;
        exit(0);
    }
    return s;
}

// prog =   func
Program* parse(Token* tokens){
    Program *prog = new Program();

    Function func;
    Statement stat;

    Token *t = tokens->next();
    while(t->get_type() != EOT){
        // cout << t->get_str()<< endl;
        Function *func = new Function();
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
        Statement *stat = statement(&t, t);
        func->statements = stat;
        prog->functions = func;
    }

    return prog;
}
    
string space(int levels){
    string spc =  "    "; 
    string ret = "";
    for(int i=0;i<levels;i++) ret += spc;
    return ret;
}
void Pretty_printing(Program* prog){
    //cout << "PROGRAM " << endl;
    Function *func = prog->functions;
    int level = 1;
    if(func != nullptr){
        cout << "FUN INT "<< func->name << ":"<<endl;
        cout<<space(level) << "params:"<<endl;
        cout <<space(level) <<"body:"<<endl;
        Statement* stat = func->statements;
        level++;
        if(stat != nullptr){
            Expression *exp = stat->exps;
            cout <<space(level) << "RETURN " << exp->val << endl;
        }
        level--;
    }
        
}


string gen_ret(Statement *stat){
    Expression *exp = stat->exps;
    string num = to_string(exp->val);
    string cmd = regex_replace("movl    $\%d, \%eax", regex("\%d"), num);
    string ret = "";
    ret += space(1)+cmd + "\n";
    cmd = "ret";
    ret += space(1)+cmd;
    return ret;
}
string codegen(Program* prog){
    string ret = "";
    string fun= ".global _$\n_$:";
    Function *func = prog->functions;
    string name = func->name;
    fun = regex_replace(fun, regex("[$]"), name);
    ret.append(fun + "\n");
    ret.append(gen_ret(func->statements));
    ret.append("\n");
    return ret;
    
}
int main(int argc, char* argv[]){
    //if(argc != 2){
        //cout << "wrong line number" <<endl;
        //return 0;
    //}
    string addr ;
    //addr = argv[1];
    addr = "./test/test.c";
    string filename = addr.substr(addr.find_last_of("/\\")+1);
    string path = string(addr.begin(), addr.end()-filename.size());
    // cout << path << endl;
    filename = string(filename.begin(),filename.end()-2);
    // cout << filename << endl;
    
    ifstream infile;
    infile.open(addr,ios::in);
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
        // t->show();
        // cout << t->get_type() << endl;
        t = t->next();
    }
    Program *prog = parse(tokens); 

    if(prog == nullptr){
        cout << "parsing errror: program"<<endl;
        exit(1);
    }
    if(prog->functions == nullptr){
        cout << "parsing error: no function" << endl;
        exit(1);
    }
    //Pretty_printing(prog);
    
    // cout << "over" << endl;
    string code =  codegen(prog);
    cout << code << endl;

    fstream outfile(path+"assembly.s", ios::out);
    outfile << code;
    outfile.close();
    
    string command = "gcc -m32 "+path+"assembly.s -o "+path+filename;
    system(command.c_str());

    string apath = path+"assembly.s";
    remove(apath.c_str());

    return 0;
}