#include <Parser/Parser.h>

#include <gtest/gtest.h>
// #include <gmock/gmock.h>
// using namespace ::testing;

// // no children version
// auto TreeEq(const Lexeme::Type &expectedType)
// {
//     return Truly([=](const std::unique_ptr<AST::Node> &root){
//         return root->lexeme.type == expectedType && root->children.empty();
//     });
// }

// template<typename ...ChildrenMatchers>
// auto TreeEq(const Lexeme::Type &expectedType, ChildrenMatchers... childrenMatcher)
// {
//     return AllOf(
//         Field(&AST::Node::lexeme, Field(&Lexeme::type, expectedType)),
//         Field(&AST::Node::children, ElementsAre(childrenMatcher...))
//     );
// }


struct TypeTree
{
    TypeTree(AST::Node::Type type, std::vector<TypeTree> children) : type(type), children(children) {}

    AST::Node::Type type;
    std::vector<TypeTree> children;
};

void ASSERT_TREE_EQ(const AST::Node *root, const TypeTree &equivilent)
{
    ASSERT_EQ(root->type, equivilent.type);
    ASSERT_EQ(root->children.size(), equivilent.children.size());
    for(size_t i = 0; i < root->children.size(); i++)
    {
        ASSERT_TREE_EQ(root->children[i].get(), equivilent.children[i]);
    }
}

// macroes to make construction easier
#define L(type) Lexeme{"", -1, -1, Lexeme::Type::type}

// The __VA_ARGS__ is the children (needed to get commas)
#define TREE(type, ...) TypeTree(AST::Node::Type::type, __VA_ARGS__)
#define TERMINAL(type) TypeTree(AST::Node::Type::type, {})

TEST(Parser, empty)
{
    AST ast({});

    ASSERT_TREE_EQ(ast.getRoot(),
        TERMINAL(Block)
    );
}

TEST(Parser, minimal)
{
    AST ast({L(Identifier), L(Semicolon)});

    ASSERT_TREE_EQ(ast.getRoot(),
        TREE(Block, {
            TERMINAL(Variable)
        })
    );
}

TEST(Parser, add)
{
    AST ast({L(Identifier), L(Plus), L(Number), L(Semicolon)});

    ASSERT_TREE_EQ(ast.getRoot(),
        TREE(Block, {
            TREE(Add, {
                TERMINAL(Variable),
                TERMINAL(Number)
            })
        })
    );
}

TEST(Parser, subtract)
{
    AST ast({L(Identifier), L(Minus), L(Number), L(Semicolon)});

    ASSERT_TREE_EQ(ast.getRoot(),
        TREE(Block, {
            TREE(Subtract, {
                TERMINAL(Variable),
                TERMINAL(Number)
            })
        })
    );
}

TEST(Parser, multiply)
{
    AST ast({L(Identifier), L(Multiply), L(Number), L(Semicolon)});

    ASSERT_TREE_EQ(ast.getRoot(),
        TREE(Block, {
            TREE(Multiply, {
                TERMINAL(Variable),
                TERMINAL(Number)
            })
        })
    );
}

TEST(Parser, divide)
{
    AST ast({L(Identifier), L(Divide), L(Number), L(Semicolon)});

    ASSERT_TREE_EQ(ast.getRoot(),
        TREE(Block, {
            TREE(Divide, {
                TERMINAL(Variable),
                TERMINAL(Number)
            })
        })
    );
}

TEST(Parser, twoAddition)
{
    AST ast({L(Identifier), L(Plus), L(Number), L(Plus), L(String), L(Semicolon)});

    ASSERT_TREE_EQ(ast.getRoot(),
        TREE(Block, {
            TREE(Add, {
                TREE(Add, {
                    TERMINAL(Variable),
                    TERMINAL(Number)
                }),
                TERMINAL(String)
            })
        })
    );
}

TEST(Parser, twoMultiplication)
{
    AST ast({L(Identifier), L(Multiply), L(Number), L(Multiply), L(String), L(Semicolon)});

    ASSERT_TREE_EQ(ast.getRoot(),
        TREE(Block, {
            TREE(Multiply, {
                TREE(Multiply, {
                    TERMINAL(Variable),
                    TERMINAL(Number)
                }),
                TERMINAL(String)
            })
        })
    );
}

TEST(Parser, functionCall_noArgs)
{
    AST ast({L(Identifier), L(LParentheses), L(RParentheses), L(Semicolon)});

    ASSERT_TREE_EQ(ast.getRoot(),
        TREE(Block, {
            TERMINAL(FunctionCall)
        })
    );
}

TEST(Parser, functionCall_oneArg)
{
    AST ast({L(Identifier), L(LParentheses), L(Number), L(RParentheses), L(Semicolon)});

    ASSERT_TREE_EQ(ast.getRoot(),
        TREE(Block, {
            TREE(FunctionCall, {
                TERMINAL(Number)
            })
        })
    );
}

TEST(Parser, functionCall_twoArgs)
{
    AST ast({L(Identifier), L(LParentheses), L(Number), L(Comma), L(String), L(RParentheses), L(Semicolon)});

    ASSERT_TREE_EQ(ast.getRoot(),
        TREE(Block, {
            TREE(FunctionCall, {
                TERMINAL(Number),
                TERMINAL(String)
            })
        })
    );
}

TEST(Parser, arithmeticOrder)
{
    AST ast({L(Identifier), L(Plus), L(Number), L(Multiply), L(String), L(Semicolon)});

    ASSERT_TREE_EQ(ast.getRoot(),
        TREE(Block, {
            TREE(Add, {
                TERMINAL(Variable),
                TREE(Multiply, {
                    TERMINAL(Number),
                    TERMINAL(String)
                })
            })
        })
    );
}

TEST(Parser, expressionOrder)
{
    AST ast({L(LParentheses), L(Identifier), L(Plus), L(Identifier), L(LParentheses), L(RParentheses), L(RParentheses), L(Multiply), L(String), L(Semicolon)});

    ASSERT_TREE_EQ(ast.getRoot(),
        TREE(Block, {
            TREE(Multiply, {
                TREE(Add, {
                    TERMINAL(Variable),
                    TERMINAL(FunctionCall)
                }),
                TERMINAL(String)
            })
        })
    );
}

TEST(Parser, ifStatement)
{
    AST ast({L(KwIf), L(Identifier), L(Equality), L(Number), L(KwBegin),
        L(Identifier), L(Assign), L(String), L(Semicolon),
        L(Identifier), L(Assign), L(Number), L(Semicolon),
        L(Identifier), L(Assign), L(Identifier), L(Semicolon),
    L(KwEnd)});

    ASSERT_TREE_EQ(ast.getRoot(),
        TREE(Block, {
            TREE(If, {
                TREE(Equals, {
                    TERMINAL(Variable),
                    TERMINAL(Number)
                }),
                TREE(Block, {
                    TREE(Assign, {
                        TERMINAL(Variable),
                        TERMINAL(String)
                    }),
                    TREE(Assign, {
                        TERMINAL(Variable),
                        TERMINAL(Number)
                    }),
                    TREE(Assign, {
                        TERMINAL(Variable),
                        TERMINAL(Variable)
                    }),
                })
            })
        })
    );
}

TEST(Parser, whileStatement)
{
    AST ast({L(KwWhile), L(Identifier), L(Equality), L(Number), L(KwBegin),
        L(Identifier), L(Assign), L(String), L(Semicolon),
    L(KwEnd)});

    ASSERT_TREE_EQ(ast.getRoot(),
        TREE(Block, {
            TREE(While, {
                TREE(Equals, {
                    TERMINAL(Variable),
                    TERMINAL(Number)
                }),
                TREE(Block, {
                    TREE(Assign, {
                        TERMINAL(Variable),
                        TERMINAL(String)
                    }),
                })
            })
        })
    );
}
