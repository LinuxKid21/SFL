#include <Interpreter/InterpreterError.h>


InterpreterError::InterpreterError(std::string error, AST::Node *node)
    : std::runtime_error(error), node(node)
{}
