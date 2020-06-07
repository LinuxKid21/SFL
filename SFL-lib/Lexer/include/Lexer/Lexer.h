#pragma once

#include <string>
#include <vector>
#include <limits>

#include <iostream> // for the << operator (TODO: move me)

struct Lexeme
{
    std::string name;
    int lineNumber;
    int colPosition;

    enum class Type : int
    {
        Error = -1,
        Identifier = 0,

        LiteralStart = 1000,
        Number,
        String,
        LiteralEnd,

        OperatorStart = 2000,
        Plus,
        Minus,
        Multiply,
        Divide,
        LParentheses,
        RParentheses,
        Comma,
        Assign,
        Equality,
        Semicolon,
        Colon,
        Period,
        OperatorEnd,

        KeywordStart = 3000,
        KwFunction,
        KwIf,
        KwWhile,
        KwBegin,
        KwEnd,
        KeywordEnd,

        EndOfFile = std::numeric_limits<int>::max()
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
