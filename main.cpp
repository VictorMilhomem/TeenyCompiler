#include <iostream>

#include "./src/compiler.h"


int main(int argc, char** argv) {

    std::string file_contents;
    file_contents = getFileContents("D:\\Projects\\TinyCompiler\\examples\\example3.tiny");

    Lexer lexer{file_contents};
    Emmiter emmiter{"out.c"};
    Parser parser{lexer, emmiter};
    parser.program();
    parser.transpile_to_c();
    std::system("gcc ./out.c ");
    std::cout << "Compiled" << std::endl;


    return 0;
}
