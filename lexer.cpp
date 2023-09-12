#include "mycc.h"

Token* lex(vector<string> buff){
    Token *tokens = new Token();
    Token *pre = tokens;
    Token test = Token("test",EOT);
    regex re_op("[\\+\\-\\*\\/\\!\\~]");
    regex re_num("[0-9]+");
    regex re_keyword("int|return|float");
    regex re_key("[\\(\\)\\{\\}\\[\\]\\;]");
    regex re("[\\(\\)\\{\\}\\[\\]\\;\\-\\!\\~]|[^\\(\\)\\{\\}\\[\\]\\;\\s]+");
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
                *token = Token(str, LITNUM);
            }
            else if(regex_match(str,re_op)){
                *token = Token(str, PUNCT);
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