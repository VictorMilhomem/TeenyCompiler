#ifndef TINYCOMPILER_PARSER_H
#define TINYCOMPILER_PARSER_H

#include <iostream>
#include <utility>
#include "lexer.h"

class Parser
{
public:
    explicit Parser(Lexer lex)
    {
        this->m_lexer = std::move(lex);
        next_token();
        next_token();
    }

private:
    Lexer m_lexer;
    Token* m_currentToken;
    Token* m_lookahead;

    void next_token()
    {
        m_currentToken = m_lookahead;
        m_lookahead = m_lexer.Scan();
    }

    bool check_token(int tag)
    {
        return tag == m_currentToken->tag;
    }

    bool check_lookahead(int tag)
    {
        return tag == m_lookahead->tag;
    }

    void match(int tag)
    {
        if (!check_token(tag))
            throw SyntaxError{"Expected " + m_lookahead->lexeme + " got " + m_currentToken->lexeme, m_lexer.Lineno()};
        next_token();
    }



};


#endif //TINYCOMPILER_PARSER_H
