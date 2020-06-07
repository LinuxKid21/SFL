#pragma once

#include <Lexer/Lexer.h>
#include <string>
#include <vector>
#include <memory>

class AST
{
public:
    struct Node
    {
        std::string stringTree() const;

        Lexeme lexeme;
        std::vector<std::unique_ptr<AST::Node> > children;
    };
    
    AST(const LexemeList &lexemes);

    // AST is the owner of this pointer
    const Node *getRoot() const;

private:
    std::unique_ptr<Node> root;
};

class ParserError : public std::runtime_error
{
public:
    ParserError(std::string error, Lexeme lexeme = {});
    Lexeme lexeme;
};
