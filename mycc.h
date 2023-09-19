#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <string.h>
#include <regex>
using std::cin,
	  std::cout,
	  std::vector,
	  std::string,
	  std::endl,
	  std::to_string,
	  std::regex,
	  std::ifstream,
	  std::regex_iterator,
	  std::ios,
	  std::next,
	  std::stoi;

class Token;
class Node;
class Function;
class Program;

//util.cpp
void err_print(string str);
bool is_number(const std::string& s);
bool consume(Token **rest, Token *t, string str);
string space(int levels);
Token* skip(Token *t, string str);
void Pretty_printing(Program* prog);


Token* lex(vector<string> buff);
Program* parse(Token* tokens);
string codegen(Program* prog);

enum token_type{
    IDENTIFER,
    KEY,
    LITNUM,
    PUNCT,
    EOT//End of tokens
};
enum data_type{
    INT,
};
enum node_type{
    NAN,//Not a node
    NUM, 
    RET,
    NEG,
    BCPL,
    LNEG,
	LT,
	LE,
	GT,
	GE,
	NE,
	OR,
	AND,
	EQUAL,
	ADD,
	SUB,
	MUL,
	DIV,
	ASSIGN,
	COMPOUND,
	EXPR,

};


class Node{
    public:
    Token* token=nullptr;
    node_type type=NAN;
	data_type dtype=INT;
    Node *parent=nullptr;
    Node *lchild=nullptr;
    Node *rchild=nullptr;
	
    int  val_int = 0;
	
    Node(){}
    Node(Token* t, node_type ty){
        type = ty;
        token = t;
    }
};

class Function{
    public:
    Node* body = nullptr;
    string name="";
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
    token_type get_type(){return type;}
    string get_str(){return str;}
    void set_next(Token* t){
        next_token = t;
    }
    Token* next(){return next_token;}
    bool check(string str){
        return this->str == str;
    } 
};
