#include <iostream>

#include "./src/compiler.h"

int main() {
    std::string file_contents;
    file_contents = getFileContents("D:\\Projects\\TinyCompiler\\examples\\example1.tiny");
    int i = 0;
    Lexer lexer{file_contents};
    char current = lexer.GetPeek();

    while (current != '\0')
    {
        Token *tk = lexer.Scan();
        std::cout << "< "+ tk->lexeme + " >" << std::endl;
        current = lexer.GetPeek();
    }
    return 0;
}
