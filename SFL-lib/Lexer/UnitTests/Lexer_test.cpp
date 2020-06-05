#include <Lexer/Lexer.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
using namespace ::testing;

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

void testSingleChar(char src)
{
    const std::string srcStr = std::string(1, src);

    ASSERT_THAT(Lexer::lexString(srcStr), 
        ElementsAre(Lexeme{srcStr, 1, 1}));

    ASSERT_THAT(Lexer::lexString(srcStr + " "), 
        ElementsAre(Lexeme{srcStr, 1, 1}));

    ASSERT_THAT(Lexer::lexString(" " + srcStr), 
        ElementsAre(Lexeme{srcStr, 1, 2}));
}

TEST(Lexer, singleAlpha)
{
    for(char i = 'a'; i <= 'z'; i++)
    {
        testSingleChar(i);
    }
    for(char i = 'A'; i <= 'Z'; i++)
    {
        testSingleChar(i);
    }
}

TEST(Lexer, singleNumeric)
{
    for(char i = '0'; i <= '9'; i++)
    {
        testSingleChar(i);
    }
}

TEST(Lexer, singleOperator)
{
    testSingleChar('(');
    testSingleChar(')');
    testSingleChar(',');
    testSingleChar('+');
    testSingleChar('-');
    testSingleChar('*');
    testSingleChar('/');
    testSingleChar(';');
}

// no longer test by category. We don't really care about that in the lexer

void testSingleLexeme(std::string src)
{
    ASSERT_THAT(Lexer::lexString(src), 
        ElementsAre(Lexeme{src, 1, 1}));

    ASSERT_THAT(Lexer::lexString(src + " "), 
        ElementsAre(Lexeme{src, 1, 1}));

    ASSERT_THAT(Lexer::lexString(" " + src), 
        ElementsAre(Lexeme{src, 1, 2}));
}

TEST(Lexer, single)
{
    testSingleLexeme("a");
    testSingleLexeme("foo");
    testSingleLexeme("?");
    testSingleLexeme("!");
    testSingleLexeme("?Foo");
    testSingleLexeme("Ha!");
    testSingleLexeme("_");
    testSingleLexeme("Under_scores");
    testSingleLexeme("ALLCAPS");
    testSingleLexeme("123");
    testSingleLexeme("a3");
    testSingleLexeme("a6");
    testSingleLexeme("3.14");
}

TEST(Lexer, singleStrings)
{
    ASSERT_THAT(Lexer::lexString(R"("")"), 
        ElementsAre(Lexeme{"", 1, 1}));

    ASSERT_THAT(Lexer::lexString(R"( "\"" )"), 
        ElementsAre(Lexeme{"\"", 1, 2}));
        
    ASSERT_THAT(Lexer::lexString(R"( "What ever  I +-*/ want 'to \"!@#$%^&*Z(123.456,789|0`)\" \t put in here" )"), 
        ElementsAre(Lexeme{"What ever  I +-*/ want 'to \"!@#$%^&*Z(123.456,789|0`)\" \t put in here", 1, 2}));

    // no termination to this string
    ASSERT_THROW(Lexer::lexString(R"( " )"), LexerError);
}

TEST(Lexer, noOperators)
{
    ASSERT_THAT(Lexer::lexString("a b c"), 
        ElementsAre(Lexeme{"a", 1, 1}, Lexeme{"b", 1, 3}, Lexeme{"c", 1, 5}));

    ASSERT_THAT(Lexer::lexString("function foo?bar baz2! 123.5"), 
        ElementsAre(Lexeme{"function", 1, 1}, Lexeme{"foo?bar", 1, 10}, Lexeme{"baz2!", 1, 18}, Lexeme{"123.5", 1, 24}));
}

TEST(Lexer, operatorStart)
{
    ASSERT_THAT(Lexer::lexString("+a"), 
        ElementsAre(Lexeme{"+", 1, 1}, Lexeme{"a", 1, 2}));

    ASSERT_THAT(Lexer::lexString("+ a"), 
        ElementsAre(Lexeme{"+", 1, 1}, Lexeme{"a", 1, 3}));

    ASSERT_THAT(Lexer::lexString("(*a"), 
        ElementsAre(Lexeme{"(", 1, 1}, Lexeme{"*", 1, 2}, Lexeme{"a", 1, 3}));
}

TEST(Lexer, operatorEnd)
{
    ASSERT_THAT(Lexer::lexString("a+"), 
        ElementsAre(Lexeme{"a", 1, 1}, Lexeme{"+", 1, 2}));

    ASSERT_THAT(Lexer::lexString("a +"), 
        ElementsAre(Lexeme{"a", 1, 1}, Lexeme{"+", 1, 3}));

    ASSERT_THAT(Lexer::lexString("abc*)"), 
        ElementsAre(Lexeme{"abc", 1, 1}, Lexeme{"*", 1, 4}, Lexeme{")", 1, 5}));
}

TEST(Lexer, examples)
{
    ASSERT_THAT(Lexer::lexString("a=1+2;"), 
        ElementsAre(Lexeme{"a", 1, 1}, Lexeme{"=", 1, 2}, Lexeme{"1", 1, 3}, Lexeme{"+", 1, 4}, Lexeme{"2", 1, 5}, Lexeme{";", 1, 6}));

    ASSERT_THAT(Lexer::lexString("  a   =   1   +   2   ;   "), 
        ElementsAre(Lexeme{"a", 1, 3}, Lexeme{"=", 1, 7}, Lexeme{"1", 1, 11}, Lexeme{"+", 1, 15}, Lexeme{"2", 1, 19}, Lexeme{";", 1, 23}));

    ASSERT_THAT(Lexer::lexString("function foo(a, b) begin return a+b; end"), 
        ElementsAre(Lexeme{"function", 1, 1}, Lexeme{"foo", 1, 10}, Lexeme{"(", 1, 13}, Lexeme{"a", 1, 14}, Lexeme{",", 1, 15},
        Lexeme{"b", 1, 17}, Lexeme{")", 1, 18}, Lexeme{"begin", 1, 20}, Lexeme{"return", 1, 26}, Lexeme{"a", 1, 33},
        Lexeme{"+", 1, 34}, Lexeme{"b", 1, 35}, Lexeme{";", 1, 36}, Lexeme{"end", 1, 38}));

    ASSERT_THAT(Lexer::lexString("a=1**2;"), 
        ElementsAre(Lexeme{"a", 1, 1}, Lexeme{"=", 1, 2}, Lexeme{"1", 1, 3}, Lexeme{"**", 1, 4}, Lexeme{"2", 1, 6}, Lexeme{";", 1, 7}));

    ASSERT_THAT(Lexer::lexString("a=1* *2;"), 
        ElementsAre(Lexeme{"a", 1, 1}, Lexeme{"=", 1, 2}, Lexeme{"1", 1, 3}, Lexeme{"*", 1, 4}, Lexeme{"*", 1, 6}, Lexeme{"2", 1, 7}, Lexeme{";", 1, 8}));
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
        Lexeme{"function", 1, 1}, Lexeme{"fib", 1, 10}, Lexeme{"(", 1, 13}, Lexeme{"N", 1, 14}, Lexeme{")", 1, 15}, Lexeme{"begin", 1, 17},
        
        Lexeme{"if", 2, 5}, Lexeme{"N", 2, 8}, Lexeme{"==", 2, 10}, Lexeme{"0", 2, 13}, Lexeme{"begin", 2, 15},
        
        Lexeme{"return", 3, 9}, Lexeme{"0", 3, 16}, Lexeme{";", 3, 17},

        Lexeme{"end", 4, 5},
        
        Lexeme{"if", 5, 5}, Lexeme{"N", 5, 8}, Lexeme{"==", 5, 10}, Lexeme{"1", 5, 13}, Lexeme{"begin", 5, 15},

        Lexeme{"return", 6, 9}, Lexeme{"1", 6, 16}, Lexeme{";", 6, 17},
        
        Lexeme{"end", 7, 5},
        
        Lexeme{"return", 9, 5}, Lexeme{"fib", 9, 12}, Lexeme{"(", 9, 15}, Lexeme{"N", 9, 16}, Lexeme{"-", 9, 17},
        Lexeme{"1", 9, 18}, Lexeme{")", 9, 19}, Lexeme{"+", 9, 21}, Lexeme{"fib", 9, 23}, Lexeme{"(", 9, 26},
        Lexeme{"N", 9, 27}, Lexeme{"-", 9, 28}, Lexeme{"2", 9, 29}, Lexeme{")", 9, 30}, Lexeme{";", 9, 31},
        
        Lexeme{"end", 10, 1})
    );
}

TEST(Lexer, comments)
{
    ASSERT_EQ(Lexer::lexString("# foo").size(), 0);

    ASSERT_THAT(Lexer::lexString("a# foo"), 
        ElementsAre(Lexeme{"a", 1, 1}));

    ASSERT_THAT(Lexer::lexString("a + 2 #foo"), 
        ElementsAre(Lexeme{"a", 1, 1}, Lexeme{"+", 1, 3}, Lexeme{"2", 1, 5}));
}

// TODO: error cases

