#include "compiler.h"

static void compile(string src)
{
    Lexer lex{src};
}

string getFileContents(string path_to_file)
{
    ifstream file;
    string file_contents;

    file.open(path_to_file);

    if (file.fail())
    {
        cout << "Could not open the file" << endl;
        return file_contents;
    }

    stringstream buffer;

    buffer << file.rdbuf();
    file_contents = buffer.str();
    file.close();

    return file_contents;
}