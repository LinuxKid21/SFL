#include <Interpreter/Value.h>
#include <Interpreter/InterpreterError.h>

#include <type_traits>

Value Value::createString(const std::string &v)
{
    return Value(Data(v));
}

Value Value::createNumber(double v)
{
    return Value(Data(v));
}

void Value::requireTypeMatch(const Value &a, const Value &b, std::string opName)
{
    if(a.data.index() != b.data.index())
    {
        throw InterpreterError("Cannot " + opName + " values with types " + a.getTypeAsString() + " and " + b.getTypeAsString());
    }
}

Value Value::add(const Value &a, const Value &b)
{
    requireTypeMatch(a, b, "addition");
    return std::visit([&b](auto&& arg1) -> Value
    {
        return Value(Data(arg1+std::get<std::decay_t<decltype(arg1)>>(b.data)));
    }, a.data);
}

Value Value::sub(const Value &a, const Value &b)
{
    requireTypeMatch(a, b, "subtraction");
    if(std::holds_alternative<std::string>(a.data))
    {
        throw InterpreterError("Cannot subtract two strings");
    }
    return Value(std::get<double>(a.data) - std::get<double>(b.data));
}

Value Value::mul(const Value &a, const Value &b)
{
    requireTypeMatch(a, b, "multiply");
    if(std::holds_alternative<std::string>(a.data))
    {
        throw InterpreterError("Cannot multiply two strings");
    }
    return Value(Data(std::get<double>(a.data) * std::get<double>(b.data)));
}

Value Value::div(const Value &a, const Value &b)
{
    requireTypeMatch(a, b, "divide");
    if(std::holds_alternative<std::string>(a.data))
    {
        throw InterpreterError("Cannot divide two strings");
    }
    return Value(Data(std::get<double>(a.data) / std::get<double>(b.data)));
}

Value Value::equals(const Value &a, const Value &b)
{
    requireTypeMatch(a, b, "check equals");
    return std::visit([&b](auto&& arg1)
    {
        return Value(arg1==std::get<std::decay_t<decltype(arg1)>>(b.data));
    }, a.data);
}

std::string Value::getTypeAsString() const
{
    if(std::holds_alternative<std::string>(data))
    {
        return "string";
    }
    else
    {
        return "number";
    }
}

std::string Value::asString() const
{
    if(std::holds_alternative<std::string>(data))
    {
        return std::get<std::string>(data);
    }
    else
    {
        auto str = std::to_string(std::get<double>(data));
        size_t dec_loc = str.find('.');
        size_t remove_loc = str.find_last_not_of('0') + 1;
        if(dec_loc == remove_loc-1) remove_loc = dec_loc; // remove decimal too if no digits afterwards

        str.erase ( remove_loc, std::string::npos );
        return str;
    }
}

bool Value::asBool() const
{
    if(std::holds_alternative<std::string>(data))
    {
        return std::get<std::string>(data) != "";
    }
    else
    {
        return std::get<double>(data) == 1;
    }
}

Value::Value(const Data &data)
    : data(data)
{}

Value::Value(const bool &data) // convert bool to number as we don't have bool type yet.
    : data(data ? 1 : 0)
{}
