#include <Parser/Parser.h>
#include <optional>

std::unique_ptr<AST::Node> makeNode(Lexeme lexeme = {}, std::vector<std::unique_ptr<AST::Node> > children = {})
{
    auto node = std::make_unique<AST::Node>();
    node->lexeme = lexeme;
    node->children = std::move(children);

    return node;
}

#ifdef PARSE_DEBUG
#define ParseLog(x) std::cout << x << " : " << current->name << "\n";
#else
#define ParseLog(x)
#endif

class Parser
{
public:
    typedef LexemeList::const_iterator iter;

    Parser(iter _start, iter _end)
    {
        current = _start;
        end = _end;

        root = makeNode(Lexeme{"", -1, -1, Lexeme::Type::KeywordStart}, program());
        if(peek() == Lexeme::Type::KwEnd)
        {
            // special case where the program() function ended seeing the 'end' keyword
            throw ParserError("Unexpected 'end' keyword found", *current);
        }
    }

    std::vector<std::unique_ptr<AST::Node> > program()
    {
        ParseLog("program");
        std::vector<std::unique_ptr<AST::Node> > statements;
        while(peek() != Lexeme::Type::KwEnd && peek() != Lexeme::Type::EndOfFile)
        {
            statements.push_back(statement());
        }
        return statements;
    }

    std::unique_ptr<AST::Node> statement()
    {
        ParseLog("statement");
        if(peek() == Lexeme::Type::KwIf)
        {
            return ifStatement();
        }
        else if(peek() == Lexeme::Type::KwWhile)
        {
            return whileStatement();
        }
        else if(peek() == Lexeme::Type::KwFunction)
        {
            throw ParserError("Not Implemented");
        }
        else if(peek(1) == Lexeme::Type::Assign)
        {
            return assignment();
        }
        else
        {
            auto exprNode = expression();
            expect(Lexeme::Type::Semicolon);
            return exprNode;
        }
    }

    std::unique_ptr<AST::Node> ifStatement()
    {
        ParseLog("ifStatement");
        auto ifLexeme = expect(Lexeme::Type::KwIf);
        auto expressionNode = expression();
        auto blockNode = block();
        std::vector<std::unique_ptr<AST::Node> > children;
        children.push_back(std::move(expressionNode));
        children.push_back(std::move(blockNode));
        return makeNode(*ifLexeme, std::move(children));
    }

    std::unique_ptr<AST::Node> whileStatement()
    {
        ParseLog("whileStatement");
        auto whileLexeme = expect(Lexeme::Type::KwWhile);
        auto expressionNode = expression();
        auto blockNode = block();
        std::vector<std::unique_ptr<AST::Node> > children;
        children.push_back(std::move(expressionNode));
        children.push_back(std::move(blockNode));
        return makeNode(*whileLexeme, std::move(children));
    }

    std::unique_ptr<AST::Node> block()
    {
        ParseLog("block");
        auto startNode = expect(Lexeme::Type::KwBegin);
        auto statements = program();
        expect(Lexeme::Type::KwEnd);
        return makeNode(*startNode, std::move(statements));
    }


    std::unique_ptr<AST::Node> assignment()
    {
        ParseLog("assignment");
        auto identifier = expect(Lexeme::Type::Identifier);
        auto assign = expect(Lexeme::Type::Assign);
        auto expressionNode = expression();
        expect(Lexeme::Type::Semicolon);
        std::vector<std::unique_ptr<AST::Node> > children;
        children.push_back(makeNode(*identifier));
        children.push_back(std::move(expressionNode));
        return makeNode(*assign, std::move(children));
    }


    std::unique_ptr<AST::Node> expression()
    {
        ParseLog("expression");
        auto rootNode = term();
        while(peek() == Lexeme::Type::Plus || peek() == Lexeme::Type::Minus)
        {
            auto lhsTermNode = std::move(rootNode);
            auto operatorLexeme = advance();
            auto rhsTermNode = term();
            std::vector<std::unique_ptr<AST::Node> > children;
            children.push_back(std::move(lhsTermNode));
            children.push_back(std::move(rhsTermNode));
            rootNode = makeNode(operatorLexeme, std::move(children));
        }

        return rootNode;
    }

    std::unique_ptr<AST::Node> term()
    {
        ParseLog("term");
        auto rootNode = factor();
        while(peek() == Lexeme::Type::Multiply || peek() == Lexeme::Type::Divide)
        {
            auto lhsFactorNode = std::move(rootNode);
            auto operatorLexeme = advance();
            auto rhsFactorNode = factor();
            std::vector<std::unique_ptr<AST::Node> > children;
            children.push_back(std::move(lhsFactorNode));
            children.push_back(std::move(rhsFactorNode));
            rootNode = makeNode(operatorLexeme, std::move(children));
        }

        return rootNode;
    }

    std::unique_ptr<AST::Node> factor()
    {
        ParseLog("factor");
        auto rootNode = booleanTerm();
        while(peek() == Lexeme::Type::Equality)
        {
            auto lhsBooleanTermNode = std::move(rootNode);
            auto operatorLexeme = advance();
            auto rhsBooleanTermNode = booleanTerm();
            std::vector<std::unique_ptr<AST::Node> > children;
            children.push_back(std::move(lhsBooleanTermNode));
            children.push_back(std::move(rhsBooleanTermNode));
            rootNode = makeNode(operatorLexeme, std::move(children));
        }

        return rootNode;
    }

    std::unique_ptr<AST::Node> booleanTerm()
    {
        ParseLog("booleanTerm");
        if(peek() == Lexeme::Type::LParentheses)
        {
            return group();
        }
        else if(peek() == Lexeme::Type::Identifier)
        {
            if(peek(1) == Lexeme::Type::LParentheses)
            {
                return functionCall();
            }
            else
            {
                return makeNode(*expect(Lexeme::Type::Identifier));
            }
        }
        else if(peek() == Lexeme::Type::Number)
        {
            return makeNode(*expect(Lexeme::Type::Number));
        }
        else if(peek() == Lexeme::Type::String)
        {
            return makeNode(*expect(Lexeme::Type::String));
        }
        else
        {
            throw ParserError("Expected expression", *current);
        }
    }

    std::unique_ptr<AST::Node> group()
    {
        ParseLog("group");
        expect(Lexeme::Type::LParentheses);
        auto expressionNode = expression();
        expect(Lexeme::Type::RParentheses);

        return expressionNode;
    }

    std::unique_ptr<AST::Node> functionCall()
    {
        ParseLog("functionCall");
        auto functionNameLexeme = expect(Lexeme::Type::Identifier);
        std::vector<std::unique_ptr<AST::Node>> children;
        expect(Lexeme::Type::LParentheses);
        bool first = true;
        while(peek() != Lexeme::Type::RParentheses)
        {
            if(first)
            {
                first = false;
            }
            else
            {
                expect(Lexeme::Type::Comma);
            }
            children.push_back(expression());
        }
        expect(Lexeme::Type::RParentheses);

        return makeNode(*functionNameLexeme, std::move(children));
    }


    std::optional<Lexeme> accept(Lexeme::Type type)
    {
        ParseLog("accept/expect: " << (int)type);
        if(current->type == type)
        {
            return std::optional<Lexeme>(advance());
        }
        return std::nullopt;
    }

    std::optional<Lexeme> expect(Lexeme::Type type)
    {
        auto lexemeOpt = accept(type);
        if(!lexemeOpt)
        {
            throw ParserError("Unexpected token. Was expecting: " + std::to_string((int)type), *current);
        }

        return lexemeOpt;
    }

    Lexeme advance()
    {
        Lexeme tmp = *current;
        current++;
        return tmp;
    }

    Lexeme::Type peek(int n = 0) const
    {
        iter peek = current;
        while(n > 0)
        {
            if(peek == end) return Lexeme::Type::EndOfFile;
            peek++;
            n--;
        }
        if(peek == end) return Lexeme::Type::EndOfFile;
        return peek->type;
    }

    iter current;
    iter end;

    std::unique_ptr<AST::Node> root;
};

std::string AST::Node::stringTree() const
{
    std::vector<std::pair<const AST::Node *, int> > stack;
    std::string output;
    stack.push_back({this, 0});
    while(stack.size() > 0)
    {
        auto pair = stack.back();
        auto node = pair.first;
        auto indent = pair.second;
        stack.pop_back();
        
        for(int i = 0;i < indent; i++) output.append("  ");

        for(auto iter = node->children.rbegin(); iter != node->children.rend(); iter++)
        {
            stack.push_back({iter->get(), indent + 1});
        }

        output.append(node->lexeme.name + "\n");
    }

    return output;
}

AST::AST(const LexemeList &lexemes)
{
    root = std::move(Parser(lexemes.begin(), lexemes.end()).root);
}

const AST::Node *AST::getRoot() const
{
    return root.get();
}

ParserError::ParserError(std::string error, Lexeme lexeme)
    : std::runtime_error(error), lexeme(lexeme)
{}
