#pragma once

#include <string>
#include <vector>

#include <iostream> // for the << operator (TODO: move me)

struct Lexeme
{
    std::string name;
    int lineNumber;
    int colPosition;

    enum class Type
    {
        Number,
        Identifier,
        Operator,
        String
    } type;

    friend std::ostream& operator<<(std::ostream& os, const Lexeme& bar) {
        return os << "{" << bar.name << ", " << bar.lineNumber << ", " << bar.colPosition << "}";
    }
};

bool operator ==(const Lexeme &a, const Lexeme &b); 

typedef std::vector<Lexeme> LexemeList;

class LexerError : public std::runtime_error
{
public:
    LexerError(std::string error, int line = -1, int col = -1);
    int line;
    int col;
};

class Lexer
{
public:
    static LexemeList lexString(const std::string &sourceCode);
    static LexemeList lexFile(const std::string &filePath);
};
