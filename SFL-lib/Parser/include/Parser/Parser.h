#pragma once

#include <Lexer/Lexer.h>
#include <string>
#include <vector>
#include <memory>

class AST
{
public:
    struct Node;
    typedef std::vector<std::unique_ptr<AST::Node> > NodeList;
    
    struct Node
    {
        std::string stringTree() const;

        Lexeme lexeme;
        NodeList children;

        enum class Type : int
        {
            Error = -1,

            Block,

            If,
            While,

            Assign,

            Add,
            Subtract,
            Multiply,
            Divide,

            Equals,

            Variable,
            Number,
            String,
            
            FunctionCall,
        } type;
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
