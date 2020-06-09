#include <Parser/Parser.h>

#include <stdexcept>
#include <string>


class InterpreterError : public std::runtime_error
{
public:
    InterpreterError(std::string error, AST::Node *node = nullptr);
    AST::Node *node;
};