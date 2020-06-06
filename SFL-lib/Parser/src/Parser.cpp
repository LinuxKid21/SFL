#include <Parser/Parser.h>


class Parser
{
public:
    typedef LexemeList::const_iterator iter;

    Parser(iter _start, iter _end)
    {
        current = _start;
        end = _end;
    }

    bool accept(Lexeme::Type type)
    {
        if(current->type == type)
        {
            advance();
            return true;
        }
        return false;
    }

    bool acceptAny(std::initializer_list<Lexeme::Type> types)
    {
        for(auto type : types)
        {
            if(accept(type)) return true;
        }
        return false;
    }

    void expect(Lexeme::Type type)
    {
        if(!accept(type))
        {
            throw ParserError("Unexpected token", *current);
        }
    }

    void advance()
    {
        current++;
    }

    iter current;
    iter end;

    std::vector<AST::Node *> statements;
};


AST::AST(const LexemeList &lexemes)
{
    root.children = Parser(lexemes.begin(), lexemes.end()).statements;
}

const AST::Node *AST::getRoot() const
{
    return &root;
}

ParserError::ParserError(std::string error, Lexeme lexeme)
    : std::runtime_error(error), lexeme(lexeme)
{}
