#include "mycc.h"

vector<string> punctuators = {
	"(",
	")",
	"[",
	"]",
	"{",
	"}",
	";"
};

vector<string> keywords = {
	"return",
	"int"
};

vector<string> oprators = {
	"+",
	"-",
	"/",
	"*",
	"<",
	">",
	"~",
	"!",
	"|",
	"&",
	"="
};

vector<string> long_op = {
	"++",
	"--",
	"||",
	"&&",
	">=",
	"<=",
	"==",
	"!="
};

bool check_long(string str){
	for(auto op : long_op)
		if(str == op) return true;
	return false;
}

string vec2str(vector<string> vec){
	string ret = "";
	for(auto i : vec){
		ret.append("\\"+i);
	}
	return ret;
}

Token* lex(vector<string> buff){
    Token *tokens = new Token();
    Token *pre = tokens;
	string op = vec2str(oprators);
	string punct = vec2str(punctuators);
    regex re_op("["+op+"]");
    regex re_num("[0-9]+");
    regex re_keyword("int|return|float");
    regex re_punct("["+punct+"]");
    regex re("["+op+punct+"]|[^"+op+punct+"\\s]+");
    for (auto line : buff){
        regex_iterator<string::iterator> reg_it(line.begin(), line.end(), re);
        regex_iterator<string::iterator> rend;
        while(reg_it != rend){
            string str = reg_it->str();
            Token *token = new Token();
            if(regex_match(str, re_punct) || regex_match(str, re_keyword)){
                *token = Token(str, KEY);
            }
            else if(regex_match(str, re_num)){
                *token = Token(str, LITNUM);
            }
            else if(regex_match(str,re_op)){
				string tmp = next(reg_it)->str();
				tmp = str+tmp;
				if(check_long(tmp)){
					*token = Token(tmp, PUNCT);
					reg_it = next(reg_it);
				}
				else *token = Token(str, PUNCT);
            }
            else{
                *token = Token(str, IDENTIFER);
            }
            pre->set_next(token);
            pre = token; 

            reg_it++;
        }
    }
    Token *t_end = new Token("EOT",EOT);
    pre->set_next(t_end);
    return tokens;
}
