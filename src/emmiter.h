#ifndef TINYCOMPILER_EMMITER_H
#define TINYCOMPILER_EMMITER_H

#include <iostream>
#include <utility>
#include <fstream>

class Emmiter
{
public:
    explicit Emmiter(std::string fullPath) : m_fullPath(std::move(fullPath))
    {
        m_code = "";
        m_header = "";
    }

    explicit Emmiter()
    {
    }

    void emit(std::string code)
    {
        m_code += code;
    }

    void emitLine(std::string code)
    {
        m_code += code + '\n';
    }

    void headerLine(std::string code)
    {
        m_header += code + '\n';
    }

    bool write_output_file()
    {
        std::ofstream output_file;

        output_file.open(m_fullPath, std::ios::out);

        if (output_file.fail())
        {
            std::cout << "Could not open the file" << std::endl;
            return true;
        }
        output_file << m_header + m_code;
        output_file.close();
        return false;
    }

private:
    std::string m_fullPath, m_code, m_header;
};

#endif //TINYCOMPILER_EMMITER_H
