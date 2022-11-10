#ifndef TINYCOMPILER_LEXER_H
#define TINYCOMPILER_LEXER_H

#include <iostream>
#include <unordered_map>
#include <sstream>
#include <string>
#include <cctype>
#include <utility>
#include "error.h"


#define DEBUG_LEXER 1

enum Tag{
    // end of line
    NEWLINE = 0,
    NUMBER = 1,
    IDENT = 2,
    STRING = 3,

    //Keywords

    LABEL = 101,
    GOTO = 102,
    PRINT = 103,
    INPUT = 104,
    LET = 105,
    IF = 106,
    THEN = 107,
    ENDIF = 108,
    WHILE = 109,
    REPEAT = 110,
    ENDWHILE = 111,

    //Operators

    EQ = 201,
    PLUS = 202,
    MINUS = 203,
    ASTERISK = 204,
    SLASH = 205,
    EQEQ = 206,
    NOTEQ = 207,
    LT = 208,
    LTEQ = 209,
    GT = 210,
    GTEQ = 211,
};

struct Token
{
    int tag;
    std::string lexeme;

    Token() : tag(0) {}
    explicit Token(char ch) : tag(int(ch)), lexeme({ch}) {}
    Token(int t, std::string s) : tag(t), lexeme(std::move(s)) {}
};

class Lexer{
public:
    explicit Lexer(std::string src) : m_src(std::move(src))
    {
        token_table["LABEL"] = Token{Tag::LABEL, "LABEL"};
        token_table["GOTO"] = Token{Tag::GOTO, "GOTO"};
        token_table["PRINT"] = Token{Tag::PRINT, "PRINT"};
        token_table["INPUT"] = Token{Tag::INPUT, "INPUT"};
        token_table["LET"] = Token{Tag::LET, "LET"};
        token_table["IF"] = Token{Tag::IF, "IF"};
        token_table["THEN"] = Token{Tag::THEN, "THEN"};
        token_table["ENDIF"] = Token{Tag::ENDIF, "ENDIF"};
        token_table["WHILE"] = Token{Tag::WHILE, "WHILE"};
        token_table["REPEAT"] = Token{Tag::REPEAT, "REPEAT"};
        token_table["ENDWHILE"] = Token{Tag::ENDWHILE, "ENDWHILE"};


        m_peek = m_src[m_current];

    }

    explicit Lexer(){}

    int Lineno() const
    {
        return m_line;
    }

    Token* Scan()
    {
        m_file_index++;
        if (m_file_index == m_src.length())
        {
            m_token = Token{EOF};
            return &m_token;
        }

        SkipWhiteSpace();

        if (m_peek ==  '\n')
        {
            m_line += 1;
            m_peek = Get(m_src);
            m_token = Token{Tag::NEWLINE, "EOL"};
            return &m_token;
        }

        // Comments
        while (m_peek == '#')
        {
            m_peek = Get(m_src);
            do
                m_peek = Get(m_src);
            while(m_peek != '\n');
            m_line += 1;
            m_peek = Get(m_src);
        }


        // Scan numbers
        if (isdigit(m_peek))
        {
            bool dot;

            std::stringstream ss;
            do {
                if (m_peek == '.')
                {
                    if (!dot)
                        dot = true;
                    else
                        break;
                }
                ss << m_peek;
                m_peek = Get(m_src);
            }while (isdigit(m_peek) || m_peek == '.');

            m_token = Token{Tag::NUMBER, ss.str()};
            return &m_token;
        }


        if (isalpha(m_peek))
        {
            std::stringstream ss;
            do
            {
                ss << m_peek;
                m_peek = Get(m_src);
            }
            while (isalpha(m_peek));

            std::string s = ss.str();
            auto pos = token_table.find(s);

            if (pos != token_table.end())
            {
                m_token = pos->second;
                return &m_token;
            }

            Token t {Tag::IDENT, s};
            token_table[s] = t;

            m_token = t;
            return &m_token;
        }



        switch (m_peek)
        {

            case '<': {
                char next = Get(m_src);
                if (next == '=') {
                    m_peek = Get(m_src);
                    m_token = Token{Tag::LTEQ, "<="};
                    return &m_token;
                } else {
                    UnGet(m_src);
                    m_peek = Get(m_src);
                    m_token = Token{Tag::LT, "<"};
                    return &m_token;
                }
            }
            case '>': {
                char next = Get(m_src);
                if (next == '=') {
                    m_peek = Get(m_src);
                    m_token = Token{Tag::GTEQ, ">="};
                    return &m_token;
                } else {
                    UnGet(m_src);
                    m_peek = Get(m_src);
                    m_token = Token{Tag::GT, ">"};
                    return &m_token;
                }
            }
            case '=':{
                char next = Get(m_src);
                if (next == '=') {
                    m_peek = Get(m_src);
                    m_token = Token{Tag::EQEQ, "=="};
                    return &m_token;
                } else {
                    UnGet(m_src);
                    m_peek = Get(m_src);
                    m_token = Token{Tag::EQ, "="};
                    return &m_token;
                }
                break;
            }
            case '!':
            {
                char next = Get(m_src);
                if (next == '=') {
                    m_peek = Get(m_src);
                    m_token = Token{Tag::NOTEQ, "!="};
                    return &m_token;
                } else
                    UnGet(m_src);

            }
            case '+':
            {
                m_peek = Get(m_src);
                m_token = Token{Tag::PLUS, "+"};
                return &m_token;
            }
            case '-':
            {
                m_peek = Get(m_src);
                m_token = Token{Tag::MINUS, "-"};
                return &m_token;
            }
            case '/':
            {
                m_peek = Get(m_src);
                m_token = Token{Tag::SLASH, "/"};
                return &m_token;
            }
            case '*':
            {
                m_peek = Get(m_src);
                m_token = Token{Tag::ASTERISK, "*"};
                return &m_token;
            }

            case '"':
            {
                std::stringstream ss;
                m_peek = Get(m_src);
                ss << m_peek;
                while (m_peek != '"' )
                {
                    if (m_peek == '\n' || m_peek == '\t' || m_peek == '\r' || m_peek == '\\' || m_peek == '%')
                    {
                        std::string msg = "Illegal character " + std::to_string(m_peek) + " in string";
                        throw SyntaxError{msg, m_line};
                    }
                    if (m_peek == EOF)
                    {
                        m_token = Token {EOF};
                        return &m_token;
                    }
                    m_peek = Get(m_src);
                    ss << m_peek;
                }


                std::string s = ss.str();
                if (s[s.length()-1] == '"')
                    s.pop_back();
                m_peek = Get(m_src);
                m_token = Token{Tag::STRING, s};
                return &m_token;

            }

        }

        m_token = Token{EOF, "EOF"};
        return &m_token;
    }

    char GetPeek() const
    {
        return m_peek;
    }

private:
    std::string m_src;
    char m_peek;
    Token m_token;
    int m_line = 1;
    int m_current = 0;
    size_t m_file_index = -1;


    std::unordered_map<std::string, Token> token_table;

    void SkipWhiteSpace()
    {
        while (m_peek == ' ' || m_peek == '\t' || m_peek == '\r')
        {
            m_peek = Get(m_src);
        }
    }

    char Get(std::string src)
    {
        m_current++;
        char peek = src.at(m_current);

        return peek;
    }

    char UnGet (std::string src)
    {
        m_current--;
        char peek = src.at(m_current);

        return peek;
    }
};

#endif //TINYCOMPILER_LEXER_H
