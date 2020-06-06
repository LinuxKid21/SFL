#include <Lexer/Lexer.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
using namespace ::testing;

MATCHER_P(LexemeEqName, name, "")
{
    return arg.name == name;
}
MATCHER_P3(LexemeEqNamePos, name, line, col, "")
{
    return arg.name == name && line == arg.lineNumber && col == arg.colPosition;
}
MATCHER_P2(LexemeEqNameType, name, type, "")
{
    return arg.name == name && type == arg.type;
}

TEST(Lexer, empty)
{
    const auto lexemes = Lexer::lexString("");
    ASSERT_EQ(lexemes.size(), 0);
}

TEST(Lexer, whitespace)
{
    ASSERT_EQ(Lexer::lexString(" ").size(), 0);
    ASSERT_EQ(Lexer::lexString("  ").size(), 0);
    ASSERT_EQ(Lexer::lexString("    ").size(), 0);
    ASSERT_EQ(Lexer::lexString("\n").size(), 0);
}

void testSingleChar(char src, Lexeme::Type type)
{
    const std::string srcStr = std::string(1, src);

    ASSERT_THAT(Lexer::lexString(srcStr), 
        ElementsAre(LexemeEqNameType(srcStr, type)));

    ASSERT_THAT(Lexer::lexString(srcStr + " "), 
        ElementsAre(LexemeEqNameType(srcStr, type)));

    ASSERT_THAT(Lexer::lexString(" " + srcStr), 
        ElementsAre(LexemeEqNameType(srcStr, type)));
}

TEST(Lexer, singleAlpha)
{
    for(char i = 'a'; i <= 'z'; i++)
    {
        testSingleChar(i, Lexeme::Type::Identifier);
    }
    for(char i = 'A'; i <= 'Z'; i++)
    {
        testSingleChar(i,  Lexeme::Type::Identifier);
    }
}

TEST(Lexer, singleNumeric)
{
    for(char i = '0'; i <= '9'; i++)
    {
        testSingleChar(i, Lexeme::Type::Number);
    }
}

TEST(Lexer, singleOperator)
{
    testSingleChar('(', Lexeme::Type::LParentheses);
    testSingleChar(')', Lexeme::Type::RParentheses);
    testSingleChar(',', Lexeme::Type::Comma);
    testSingleChar('+', Lexeme::Type::Plus);
    testSingleChar('-', Lexeme::Type::Minus);
    testSingleChar('*', Lexeme::Type::Multiply);
    testSingleChar('/', Lexeme::Type::Divide);
    testSingleChar(';', Lexeme::Type::Semicolon);
}

// no longer test by category. We don't really care about that in the lexer

void testSingleLexeme(std::string src, Lexeme::Type type)
{
    ASSERT_THAT(Lexer::lexString(src), 
        ElementsAre(LexemeEqNameType(src, type)));

    ASSERT_THAT(Lexer::lexString(src + " "), 
        ElementsAre(LexemeEqNameType(src, type)));

    ASSERT_THAT(Lexer::lexString(" " + src), 
        ElementsAre(LexemeEqNameType(src, type)));
}

TEST(Lexer, single)
{
    testSingleLexeme("a", Lexeme::Type::Identifier);
    testSingleLexeme("foo", Lexeme::Type::Identifier);
    testSingleLexeme("?", Lexeme::Type::Identifier);
    testSingleLexeme("!", Lexeme::Type::Identifier);
    testSingleLexeme("?Foo", Lexeme::Type::Identifier);
    testSingleLexeme("Ha!", Lexeme::Type::Identifier);
    testSingleLexeme("_", Lexeme::Type::Identifier);
    testSingleLexeme("Under_scores", Lexeme::Type::Identifier);
    testSingleLexeme("ALLCAPS", Lexeme::Type::Identifier);
    testSingleLexeme("123", Lexeme::Type::Number);
    testSingleLexeme("a3", Lexeme::Type::Identifier);
    testSingleLexeme("a6", Lexeme::Type::Identifier);
    testSingleLexeme("3.14", Lexeme::Type::Number);
    testSingleLexeme(".2", Lexeme::Type::Number);

    EXPECT_THROW(Lexer::lexString("."), LexerError);
    EXPECT_THROW(Lexer::lexString(".."), LexerError);
    EXPECT_THROW(Lexer::lexString(".1."), LexerError);
    EXPECT_THROW(Lexer::lexString("1.1."), LexerError);
}

TEST(Lexer, singleStrings)
{
    ASSERT_THAT(Lexer::lexString(R"("")"), 
        ElementsAre(LexemeEqNamePos("", 1, 1)));

    ASSERT_THAT(Lexer::lexString(R"( "\"" )"), 
        ElementsAre(LexemeEqNamePos("\"", 1, 2)));
        
    ASSERT_THAT(Lexer::lexString(R"( "What ever  I +-*/ want 'to \"!@#$%^&*Z(123.456,789|0`)\" \t put in here" )"), 
        ElementsAre(LexemeEqName("What ever  I +-*/ want 'to \"!@#$%^&*Z(123.456,789|0`)\" \t put in here")));

    // no termination to this string
    ASSERT_THROW(Lexer::lexString(R"( " )"), LexerError);
}

TEST(Lexer, noOperators)
{
    ASSERT_THAT(Lexer::lexString("a b c"), 
        ElementsAre(LexemeEqNamePos("a", 1, 1), LexemeEqNamePos("b", 1, 3), LexemeEqNamePos("c", 1, 5)));

    ASSERT_THAT(Lexer::lexString("function foo?bar baz2! 123.5"), 
        ElementsAre(LexemeEqNamePos("function", 1, 1), LexemeEqNamePos("foo?bar", 1, 10), LexemeEqNamePos("baz2!", 1, 18), LexemeEqNamePos("123.5", 1, 24)));
}

TEST(Lexer, operatorStart)
{
    ASSERT_THAT(Lexer::lexString("+a"), 
        ElementsAre(LexemeEqNamePos("+", 1, 1), LexemeEqNamePos("a", 1, 2)));

    ASSERT_THAT(Lexer::lexString("+ a"), 
        ElementsAre(LexemeEqNamePos("+", 1, 1), LexemeEqNamePos("a", 1, 3)));

    ASSERT_THAT(Lexer::lexString("(*a"), 
        ElementsAre(LexemeEqNamePos("(", 1, 1), LexemeEqNamePos("*", 1, 2), LexemeEqNamePos("a", 1, 3)));
}

TEST(Lexer, operatorEnd)
{
    ASSERT_THAT(Lexer::lexString("a+"), 
        ElementsAre(LexemeEqNamePos("a", 1, 1), LexemeEqNamePos("+", 1, 2)));

    ASSERT_THAT(Lexer::lexString("a +"), 
        ElementsAre(LexemeEqNamePos("a", 1, 1), LexemeEqNamePos("+", 1, 3)));

    ASSERT_THAT(Lexer::lexString("abc*)"), 
        ElementsAre(LexemeEqNamePos("abc", 1, 1), LexemeEqNamePos("*", 1, 4), LexemeEqNamePos(")", 1, 5)));
}

TEST(Lexer, examples)
{
    ASSERT_THAT(Lexer::lexString("a=1+2;"), 
        ElementsAre(LexemeEqNamePos("a", 1, 1), LexemeEqNamePos("=", 1, 2), LexemeEqNamePos("1", 1, 3), LexemeEqNamePos("+", 1, 4), LexemeEqNamePos("2", 1, 5), LexemeEqNamePos(";", 1, 6)));

    ASSERT_THAT(Lexer::lexString("  a   =   1   +   2   ;   "), 
        ElementsAre(LexemeEqNamePos("a", 1, 3), LexemeEqNamePos("=", 1, 7), LexemeEqNamePos("1", 1, 11), LexemeEqNamePos("+", 1, 15), LexemeEqNamePos("2", 1, 19), LexemeEqNamePos(";", 1, 23)));

    ASSERT_THAT(Lexer::lexString("function foo(a, b) begin return a+b; end"), 
        ElementsAre(LexemeEqNamePos("function", 1, 1), LexemeEqNamePos("foo", 1, 10), LexemeEqNamePos("(", 1, 13), LexemeEqNamePos("a", 1, 14), LexemeEqNamePos(",", 1, 15),
        LexemeEqNamePos("b", 1, 17), LexemeEqNamePos(")", 1, 18), LexemeEqNamePos("begin", 1, 20), LexemeEqNamePos("return", 1, 26), LexemeEqNamePos("a", 1, 33),
        LexemeEqNamePos("+", 1, 34), LexemeEqNamePos("b", 1, 35), LexemeEqNamePos(";", 1, 36), LexemeEqNamePos("end", 1, 38)));

    ASSERT_THAT(Lexer::lexString("a=1**2;"), 
        ElementsAre(LexemeEqNamePos("a", 1, 1), LexemeEqNamePos("=", 1, 2), LexemeEqNamePos("1", 1, 3), LexemeEqNamePos("**", 1, 4), LexemeEqNamePos("2", 1, 6), LexemeEqNamePos(";", 1, 7)));

    ASSERT_THAT(Lexer::lexString("a=1* *2;"), 
        ElementsAre(LexemeEqNamePos("a", 1, 1), LexemeEqNamePos("=", 1, 2), LexemeEqNamePos("1", 1, 3), LexemeEqNamePos("*", 1, 4), LexemeEqNamePos("*", 1, 6), LexemeEqNamePos("2", 1, 7), LexemeEqNamePos(";", 1, 8)));
}

TEST(Lexer, multiline)
{
    const auto src = R"(function fib(N) begin
    if N == 0 begin
        return 0;
    end
    if N == 1 begin
        return 1;
    end

    return fib(N-1) + fib(N-2);
end)";

    ASSERT_THAT(Lexer::lexString(src), ElementsAre(
        LexemeEqNamePos("function", 1, 1), LexemeEqNamePos("fib", 1, 10), LexemeEqNamePos("(", 1, 13), LexemeEqNamePos("N", 1, 14), LexemeEqNamePos(")", 1, 15), LexemeEqNamePos("begin", 1, 17),
        
        LexemeEqNamePos("if", 2, 5), LexemeEqNamePos("N", 2, 8), LexemeEqNamePos("==", 2, 10), LexemeEqNamePos("0", 2, 13), LexemeEqNamePos("begin", 2, 15),
        
        LexemeEqNamePos("return", 3, 9), LexemeEqNamePos("0", 3, 16), LexemeEqNamePos(";", 3, 17),

        LexemeEqNamePos("end", 4, 5),
        
        LexemeEqNamePos("if", 5, 5), LexemeEqNamePos("N", 5, 8), LexemeEqNamePos("==", 5, 10), LexemeEqNamePos("1", 5, 13), LexemeEqNamePos("begin", 5, 15),

        LexemeEqNamePos("return", 6, 9), LexemeEqNamePos("1", 6, 16), LexemeEqNamePos(";", 6, 17),
        
        LexemeEqNamePos("end", 7, 5),
        
        LexemeEqNamePos("return", 9, 5), LexemeEqNamePos("fib", 9, 12), LexemeEqNamePos("(", 9, 15), LexemeEqNamePos("N", 9, 16), LexemeEqNamePos("-", 9, 17),
        LexemeEqNamePos("1", 9, 18), LexemeEqNamePos(")", 9, 19), LexemeEqNamePos("+", 9, 21), LexemeEqNamePos("fib", 9, 23), LexemeEqNamePos("(", 9, 26),
        LexemeEqNamePos("N", 9, 27), LexemeEqNamePos("-", 9, 28), LexemeEqNamePos("2", 9, 29), LexemeEqNamePos(")", 9, 30), LexemeEqNamePos(";", 9, 31),
        
        LexemeEqNamePos("end", 10, 1))
    );
}

TEST(Lexer, comments)
{
    ASSERT_EQ(Lexer::lexString("# foo").size(), 0);

    ASSERT_THAT(Lexer::lexString("a# foo"), 
        ElementsAre(LexemeEqNamePos("a", 1, 1)));

    ASSERT_THAT(Lexer::lexString("a + 2 #foo"), 
        ElementsAre(LexemeEqNamePos("a", 1, 1), LexemeEqNamePos("+", 1, 3), LexemeEqNamePos("2", 1, 5)));
}

// TODO: error cases

