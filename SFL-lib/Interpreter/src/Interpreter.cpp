#include <Interpreter/Interpreter.h>
#include <Interpreter/InterpreterError.h>

#include <vector>
#include <optional>


class InterpreterImpl
{
public:
    InterpreterImpl(Interpreter &interpreter) : interpreter(interpreter) {};

    void block(const AST::Node *root)
    {
        verifyType(root, AST::Node::Type::Block);
        for(const auto &child : root->children)
        {
            statement(child.get());
        }
    }

    void statement(const AST::Node *root)
    {
        if(root->type == AST::Node::Type::Assign)
        {
            assign(root);
        }
        else if(root->type == AST::Node::Type::FunctionCall)
        {
            // our one function for now :)
            if(root->lexeme.name == "print")
            {
                for(const auto &child : root->children)
                {
                    std::cout << expression(child.get()).asString();
                }
            }
            else
            {
                throw 0; // TODO
            }
        }
        else if(root->type == AST::Node::Type::If)
        {
            if(expression(root->children[0].get()).asBool())
            {
                block(root->children[1].get());
            }
        }
        else if(root->type == AST::Node::Type::While)
        {
            while(expression(root->children[0].get()).asBool())
            {
                block(root->children[1].get());
            }
        }
        else
        {
            throw 0; // TODO
        }
    }

    void assign(const AST::Node *root)
    {
        verifyType(root, AST::Node::Type::Assign);
        interpreter.setGlobalVariable(root->children[0]->lexeme.name, expression(root->children[1].get()));
    }

    Value expression(const AST::Node *root)
    {
        auto valueOpt = value(root);
        if(valueOpt)
        {
            return *valueOpt;
        }
        
        // must be an expression then
        
        if(root->type == AST::Node::Type::Add)
        {
            return Value::add(expression(root->children[0].get()), expression(root->children[1].get()));
        }
        else if(root->type == AST::Node::Type::Subtract)
        {
            return Value::sub(expression(root->children[0].get()), expression(root->children[1].get()));
        }
        else if(root->type == AST::Node::Type::Multiply)
        {
            return Value::mul(expression(root->children[0].get()), expression(root->children[1].get()));
        }
        else if(root->type == AST::Node::Type::Divide)
        {
            return Value::div(expression(root->children[0].get()), expression(root->children[1].get()));
        }
        else if(root->type == AST::Node::Type::Equals)
        {
            return Value::equals(expression(root->children[0].get()), expression(root->children[1].get()));
        }
        else
        {
            throw 0; // TODO
        }
    }

    std::optional<Value> value(const AST::Node *root)
    {
        if(root->type == AST::Node::Type::Number)
        {
            return std::optional<Value>(Value::createNumber(std::stod(root->lexeme.name)));
        }
        else if(root->type == AST::Node::Type::String)
        {
            return std::optional<Value>(Value::createString(root->lexeme.name));
        }
        else if(root->type == AST::Node::Type::Variable)
        {
            return std::optional<Value>(interpreter.getGlobalVariable(root->lexeme.name));
        }
        else
        {
            return std::nullopt;
        }
    }

private:
    void verifyType(const AST::Node *root, AST::Node::Type type)
    {
        if(root->type != type)
        {
            throw InterpreterError("Internal error: expected different AST::Node type");
        }
    }

    Interpreter &interpreter;
};

void Interpreter::run(const AST &ast)
{
    InterpreterImpl(*this).block(ast.getRoot());
}

Value Interpreter::getGlobalVariable(const std::string &name) const
{
    if(globals.count(name) == 1)
    {
        return globals.at(name);
    }
    else
    {
        throw InterpreterError("Could not find a variable by the name " + name);
    }
}

void Interpreter::setGlobalVariable(const std::string &name, Value value)
{
    globals.insert({name, value});
}
