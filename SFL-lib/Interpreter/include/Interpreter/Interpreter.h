#pragma once

#include <Interpreter/Value.h>

#include <Parser/Parser.h>

#include <unordered_map>

class Interpreter
{
public:
    void run(const AST &ast);

    Value getGlobalVariable(const std::string &name) const;
    void setGlobalVariable(const std::string &name, Value value);

private:
    std::unordered_map<std::string, Value> globals;
};
