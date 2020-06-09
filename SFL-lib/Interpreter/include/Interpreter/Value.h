#pragma once

#include <string>
#include <variant>

class Value
{
public:
    static Value createString(const std::string &v);
    static Value createNumber(double v);

    static Value add(const Value &a, const Value &b);
    static Value sub(const Value &a, const Value &b);
    static Value mul(const Value &a, const Value &b);
    static Value div(const Value &a, const Value &b);
    static Value equals(const Value &a, const Value &b);

    std::string getTypeAsString() const;
    std::string asString() const;
    bool asBool() const;

private:
    typedef std::variant<double, std::string> Data;
    Data data;

    static void requireTypeMatch(const Value &a, const Value &b, std::string opName);

    Value() = delete;
    explicit Value(const Data &data);
    explicit Value(const bool &data);
};
