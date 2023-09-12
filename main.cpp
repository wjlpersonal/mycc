#include <mycc.h>

int main(int argc, char* argv[]){
    //if(argc != 2) err_print("wrong argc");
    string addr;
    //addr = argv[1];
    addr = "./test/test.c";
    string filename = addr.substr(addr.find_last_of("/\\")+1);
    string path = string(addr.begin(), addr.end()-filename.size());
    filename = string(filename.begin(),filename.end()-2);
    
    ifstream infile;
    infile.open(addr,ios::in);
    if(!infile.is_open()) err_print("can't open file");
    string line;
    vector<string> buff;
    while(getline(infile,line)){
        buff.push_back(line);
    }
    infile.close();

    Token *tokens = lex(buff);
    Token *t = tokens;
    while(t->get_type() != EOT){
        cout << t->get_str() <<endl;
        t = t->next();
    }
    Program *prog = parse(tokens); 

    if(prog == nullptr) err_print( "parsing errror: program");
    if(prog->functions == nullptr) err_print("parsing error: no function");
    Pretty_printing(prog);
    
    string code =  codegen(prog);
    cout << endl<<"codegen:"<<endl << code << endl;

    fstream outfile(path+"assembly.s", ios::out);
    outfile << code;
    outfile.close();
    
    string command = "gcc -m32 "+path+"assembly.s -o "+path+filename;
    system(command.c_str());

    string apath = path+"assembly.s";
    remove(apath.c_str());

    return 0;
}