#include <iostream>

#include "./src/compiler.h"


int main(int argc, char** argv) {

    if (argc == 2) {

        std::string file_contents;
        std::string fullPath = argv[1];
        file_contents = getFileContents(fullPath);
        file_contents += '\0';
        Lexer lexer{file_contents};
        Emmiter emmiter{"out.c"};
        Parser parser{lexer, emmiter};
        parser.program();
        parser.transpile_to_c();
        std::system("gcc ./out.c ");
        std::cout << "Compilation " + fullPath + " finished"<< std::endl;
    }


    return 0;
}
