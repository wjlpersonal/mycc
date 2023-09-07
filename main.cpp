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
    NUM,
    PUNC
};

enum node_type{
    FIL,
    INT,
    FLOAT,
    ADD,
    SUB,
    MUL,
    DIV,
    SEMI,
    RET,
    FUN,
    VAR,
    LBRACE,
    RBRACE,
    

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
        Node(Node* parent ,string str, node_type type){
            this->parent = parent;
            this->str = str;
            this->type = type;
        }
};

class Expression{
    public:
        int val_int;
        
};

class Statement{
    public:
        vector<Expression> expressions;
};

class Function{
    public:
        vector<Statement> statements;
    
};

class Program{
    public:
        vector<Function> functions;
};

class Token{
    private:
        string str="";
        token_type type=KEY;

    public:
        Token(){}
        Token(string str, token_type type){
            this->str = str;
            this->type = type;
        }
        void show(){cout << str << " : " << type << endl;} 
        token_type get_type(){return type;}
        string get_str(){return this->str;}
};

int main(void){
    ifstream infile;
    infile.open("C:\\data\\code\\mycc\\return_2.c",ios::in);
    if(!infile.is_open()){
        cout << "false"<<endl;
        return 0;
    }
    string line;
    vector<string> buff;
    while(getline(infile,line)){
        buff.push_back(line);
    }
    
    vector<Token> tokens;
    vector<string> token_strs;
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
            Token token;
            if(regex_match(str, re_key) || regex_match(str, re_keyword)){
                token = Token(str, KEY);
            }
            else if(regex_match(str, re_num)){
                token = Token(str, NUM);
            }
            else{
                token = Token(str, IDENTIFER);
            }
            tokens.push_back(token);

            token_strs.push_back(str);
            reg_it++;
        }
    }
    
    // for(auto token: tokens){
    //     token.show();
    // }
    
    vector<Node> nodes;
    Node root(nullptr,"root",FIL);
    nodes.push_back(root);
    Node parent = root;
    for(auto it=tokens.begin(); it!=tokens.end();it++){
        Node node;
        string str = it->get_str();
        token_type t_type = it->get_type();
        if(it->get_type() == IDENTIFER){
            if(next(it,1)->get_str()=="("){
                // cout << str << endl;
                node = Node(&parent, str, FUN);
            }
            else{
                node = Node(&parent, str, VAR);
            }
        }
        else if(it->get_type() == KEY){
            if(str == "int") node = Node(&parent, str, INT);
            if(str == "return") node = Node(&parent, str, RET);
            if(str == "(") node = Node(&parent, str, LBRACE);
        }
    }
    



    cout << "over" << endl;

    infile.close();
    
    return 0;
}

// 