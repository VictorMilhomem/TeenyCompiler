#ifndef TINYCOMPILER_ERROR_H
#define TINYCOMPILER_ERROR_H

#include <iostream>
#include <string>


class SyntaxError : std::exception
{
public:
    SyntaxError(std::string msg, int line) : m_msg(msg), m_line(line)
    {

    }
    void PrintSyntaxError()
    {
        std::cout << "SyntaxError at line :: " + std::to_string(m_line) + " " + m_msg << std::endl;
    }
private:
    std::string m_msg;
    int m_line;
};


#endif //TINYCOMPILER_ERROR_H
