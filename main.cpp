#include "mycc.h"
#include <glob.h>

vector<string> globVector(const string& pattern){
    glob_t glob_result;
    glob(pattern.c_str(),GLOB_TILDE,NULL,&glob_result);
    vector<string> files;
    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        files.push_back(string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return files;
}

int compile(string addr){
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
	/*
    Token *t = tokens;
    while(t->get_type() != EOT){
        cout << t->get_str() <<endl;
        t = t->next();
    }
	*/

    Program *prog = parse(tokens); 

    Pretty_printing(prog);
	
	/*
    string code =  codegen(prog);
    cout << endl<<"codegen:"<<endl << code << endl;

    fstream outfile(path+"assembly.s", ios::out);
    outfile << code;
    outfile.close();
    
    string command = "gcc -m32 "+path+"assembly.s -o "+path+filename;
    system(command.c_str());

    string apath = path+"assembly.s";
    remove(apath.c_str());
	*/


    return 0;
}
	
int main(int argc, char* argv[]){
	if(argc == 1){
		vector<string> files = globVector("./test/stage_4/valid/*");
		for(auto file: files){
			cout <<file <<endl;
			compile(file);
		}
	}
	if(argc > 2) err_print("too many args");
	string path = argv[1];
	vector<string> files = globVector(path);
	for(auto file: files){
		cout <<file <<endl;
		compile(file);
	}	
	return 0;
}
