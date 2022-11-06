#ifndef TINYCOMPILER_PARSER_H
#define TINYCOMPILER_PARSER_H

#include <iostream>
#include <set>
#include <utility>
#include "lexer.h"
#include "emmiter.h"

extern std::ifstream fin;

class Parser
{
public:
    explicit Parser(Lexer lex, Emmiter emmiter)
    {
        this->m_lexer = std::move(lex);
        this->m_emmiter = std::move(emmiter);

        next_token();
    }

    void program()
    {
        m_emmiter.headerLine("#include <stdio.h>");
        m_emmiter.headerLine("int main(void){");


        while (check_lookahead(Tag::NEWLINE))
        {
            next_token();
        }

        while(!check_lookahead(EOF))
        {
            try{
                statement();
            }catch (const std::exception& e)
            {
                break;
            }

        }

        m_emmiter.emitLine("return 0;");
        m_emmiter.emitLine("}");


        for (auto& label:m_labelGotoed)
        {
            auto pos = m_labelDeclared.find(label);
            if (!(pos != m_labelDeclared.end()))
                throw SyntaxError{"Attempting to GOTO an undeclared label", m_lexer.Lineno()};
        }
    }

    bool transpile_to_c()
    {
        return m_emmiter.write_output_file();
    }

private:
    Lexer m_lexer;
    Emmiter m_emmiter;
    std::set<std::string> m_symTable, m_labelDeclared, m_labelGotoed;
    Token* m_lookahead ;

    void next_token()
    {
        m_lookahead = m_lexer.Scan();
    }


    bool check_lookahead(int tag)
    {
        return tag == m_lookahead->tag;
    }

    void match(int tag)
    {
        if (!check_lookahead(tag))
            throw SyntaxError{"Expected " + std::to_string(tag) + " got " + m_lookahead->lexeme, m_lexer.Lineno()};
        next_token();
    }

    void statement()
    {
        switch (m_lookahead->tag) {
            case Tag::PRINT:
                next_token();
                if (check_lookahead(Tag::STRING))
                {
                    m_emmiter.emitLine("printf(\"" + m_lookahead->lexeme + "\\n\");");
                    next_token();
                }
                else
                {
                    m_emmiter.emit("printf(\"%");
                    m_emmiter.emit(".2f\\n\", (float)(");

                    expression();

                    m_emmiter.emitLine("));");
                }

                break;

            case Tag::IF:
                m_emmiter.emit("if(");
                next_token(); comparison();
                match(Tag::THEN);
                new_line();
                m_emmiter.emitLine("){");
                while(!check_lookahead(Tag::ENDIF))
                    statement();
                m_emmiter.emitLine("}");
                match(Tag::ENDIF);

                break;

            case Tag::WHILE:
                m_emmiter.emit("while(");
                next_token(); comparison();
                match(Tag::REPEAT);
                new_line();
                m_emmiter.emitLine("){");
                while(!check_lookahead(Tag::ENDWHILE))
                    statement();
                m_emmiter.emitLine("}");
                match(Tag::ENDWHILE);

                break;
            case Tag::LABEL: {
                next_token();
                auto pos = m_labelDeclared.find(m_lookahead->lexeme);
                if (pos != m_labelDeclared.end())
                    throw SyntaxError{"Label already declared", m_lexer.Lineno()};
                m_labelDeclared.insert(m_lookahead->lexeme);
                m_emmiter.emitLine(m_lookahead->lexeme + ":");
                match(Tag::IDENT);
                break;
            }
            case Tag::GOTO:
                next_token();
                m_labelGotoed.insert(m_lookahead->lexeme);
                m_emmiter.emitLine("goto " + m_lookahead->lexeme + ";");
                match(Tag::IDENT);
                break;
            case Tag::LET: {
                next_token();

                auto pos = m_symTable.find(m_lookahead->lexeme);
                if (!(pos != m_symTable.end())) {
                    m_symTable.insert(m_lookahead->lexeme);
                    m_emmiter.headerLine("float " + m_lookahead->lexeme + ";");

                }
                m_emmiter.emit(m_lookahead->lexeme + " = ");
                match(Tag::IDENT);
                match(Tag::EQ);
                expression();
                m_emmiter.emitLine(";");
                break;
            }
            case Tag::INPUT: {
                next_token();
                auto pos = m_symTable.find(m_lookahead->lexeme);
                if (!(pos != m_symTable.end())) {
                    m_symTable.insert(m_lookahead->lexeme);
                    m_emmiter.headerLine("float " + m_lookahead->lexeme + ";");
                }
                m_emmiter.emit("if(0 == scanf(\"%");
                m_emmiter.emitLine( "f\", &" + m_lookahead->lexeme + ")) {");
                m_emmiter.emitLine(m_lookahead->lexeme + " = 0;");
                m_emmiter.emit("scanf(\"%");
                m_emmiter.emitLine("*s\");");
                m_emmiter.emitLine("}");
                match(Tag::IDENT);
                break;
            }
            default: throw SyntaxError{"Invalid statement " + m_lookahead->lexeme, m_lexer.Lineno()};
        }
        new_line();
    }

    void new_line()
    {
        match(Tag::NEWLINE);
        while(check_lookahead(Tag::NEWLINE))
            next_token();
    }

    void expression()
    {
        term();
        while(check_lookahead(Tag::PLUS) || check_lookahead(Tag::MINUS))
        {
            m_emmiter.emit(m_lookahead->lexeme);
            next_token();
            term();
        }
    }

    void term()
    {
        unary();
        while (check_lookahead(Tag::ASTERISK) || check_lookahead(Tag::SLASH))
        {
            m_emmiter.emit(m_lookahead->lexeme);
            next_token();
            unary();
        }
    }

    void unary()
    {

        if (check_lookahead(Tag::PLUS) || check_lookahead(Tag::MINUS)) {
            m_emmiter.emit(m_lookahead->lexeme);
            next_token();
        }
        primary();
    }

    void primary()
    {
        switch (m_lookahead->tag) {
            case Tag::NUMBER:
                m_emmiter.emit(m_lookahead->lexeme);
                next_token();
                break;
            case Tag::IDENT: {
                auto pos = m_symTable.find(m_lookahead->lexeme);
                if (!(pos != m_symTable.end()))
                    throw SyntaxError{"Referencing variable before assigned", m_lexer.Lineno()};
                m_emmiter.emit(m_lookahead->lexeme);
                next_token();
                break;
            }
            default:
                throw SyntaxError{"Unexpected Token ", m_lexer.Lineno()};
        }
    }

    void comparison()
    {
        expression();
        if (isComparisonOp())
        {
            m_emmiter.emit(m_lookahead->lexeme);
            next_token();
            expression();
        }else
            throw SyntaxError{"Expected Comparison operator", m_lexer.Lineno()};

        while (isComparisonOp())
        {
            m_emmiter.emit(m_lookahead->lexeme);
            next_token(); expression();
        }

    }

    bool isComparisonOp()
    {
        return check_lookahead(Tag::GT) || check_lookahead(Tag::GTEQ) || check_lookahead(Tag::LT) || check_lookahead(Tag::LTEQ) || check_lookahead(Tag::EQEQ) || check_lookahead(Tag::NOTEQ);
    }

};


#endif //TINYCOMPILER_PARSER_H
