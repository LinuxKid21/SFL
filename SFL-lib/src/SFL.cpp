#include <SFL/SFL.h>

#include <Parser/Parser.h>
#include <Interpreter/Interpreter.h>

#include <string>
#include <iostream>

void SFL::test()
{
    try
    {
        std::string line;
        std::string program;
        while(line != "EOF")
        {
            std::cin >> line;
            if(line != "EOF")
            {
                program.append(line + "\n");
            }
        }

        AST ast(Lexer::lexString(program));
        Interpreter().run(ast);
        // std::cout << ast.getRoot()->stringTree() << std::endl;
    }
    catch(const ParserError& e)
    {
        std::cerr << e.what() << '\n';
        std::cerr << "name (" << e.lexeme.name << ") line:col (" << e.lexeme.lineNumber << ":" << e.lexeme.colPosition << ") type (" << (int)e.lexeme.type << ")" << '\n';
    }
}
