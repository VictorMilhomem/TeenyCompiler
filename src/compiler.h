#ifndef TINYCOMPILER_COMPILER_H
#define TINYCOMPILER_COMPILER_H

#include "lexer.h"
#include <fstream>
using  namespace std;

static void compile(string src);

string getFileContents(string path_to_file);


#endif //TINYCOMPILER_COMPILER_H
