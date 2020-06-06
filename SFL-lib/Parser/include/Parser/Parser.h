#pragma once

#include <Lexer/Lexer.h>
#include <string>
#include <vector>

class AST
{
public:
    struct Node
    {
        Lexeme lexeme;
        std::vector<Node *> children;
    };
    
    AST(const LexemeList &lexemes);

    // AST is the owner of this pointer
    const Node *getRoot() const;

private:
    Node root;
};

class ParserError : public std::runtime_error
{
public:
    ParserError(std::string error, Lexeme lexeme = {});
    Lexeme lexeme;
};
