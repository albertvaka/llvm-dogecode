#include <iostream>
#include <cstdio>
#include "codegen.h"
#include "node.h"

using namespace std;

extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern NBlock* programBlock;
void createCoreFunctions(CodeGenContext& context);

int main(int argc, char **argv)
{
    if (argc != 2) {
        cout << "Wrong num of args" << endl;
        return -1;
    }
    FILE *myfile = fopen(argv[1], "r");
    if (!myfile) {
        cout << "I can't open: " << argv[1] << endl;
        return -1;
    }
    yyin = myfile;

	yyparse();
	std::cout << programBlock << endl;
    // see http://comments.gmane.org/gmane.comp.compilers.llvm.devel/33877
	InitializeNativeTarget();
	CodeGenContext context;
	createCoreFunctions(context);
	context.generateCode(*programBlock);
	context.runCode();
	
	return 0;
}

