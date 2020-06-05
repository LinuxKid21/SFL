#include <Lexer/Lexer.h>
#include <fstream>
#include <sstream>
#include <ctype.h> // isalpha, isspace, etc...

// ----- implementation functions -----

class LexemeImpl
{
public:
    typedef std::string::const_iterator iter;
    LexemeImpl(iter _start, iter _end)
    {
        start = _start;
        end = _end;

        skipWhitespace();
        while(!isDone())
        {
            int wordLine = line;
            int wordCol = col;

            char c = peek();
            if(isWordChar(c))
            {
                lexemes.push_back(Lexeme{getWord(), wordLine, wordCol});
            }
            else if(isToken(c))
            {
                advance(); // eat the token

                if(c == '*' && peek() == '*')
                {
                    advance();
                    lexemes.push_back(Lexeme{"**", wordLine, wordCol});
                }
                else if(c == '=' && peek() == '=')
                {
                    advance();
                    lexemes.push_back(Lexeme{"==", wordLine, wordCol});
                }
                else
                {
                    lexemes.push_back(Lexeme{std::string(1, c), wordLine, wordCol});
                }
            }
            else if(c == '"')
            {
                lexemes.push_back(Lexeme{getString(), wordLine, wordCol});
            }
            else if(c == '#')
            {
                eatComment();
            }
            else
            {
                throw LexerError("Unexpected token", wordLine, wordCol);
            }
            
            skipWhitespace();
        }
    }

    bool isWordChar(char c) const
    {
        return isalnum(c) || c == '_' || c == '?' || c == '!' || c == '.';
    }

    // split into getWord and getNumber
    // figure out '.' ambiguity (operator + decimal)
    std::string getWord()
    {
        iter wordStart = start;
        while(isWordChar(peek()))
        {
            advance();
        }
        return std::string(wordStart, start);
    }

    std::string getString()
    {
        std::string output;
        advance(); // eat starting quote
        while(peek() != '"')
        {
            if(peek() == '\0')
            {
                throw LexerError("String did not terminate");
            }

            if(peek() == '\\')
            {
                advance();
                char escaped = advance();
                if(escaped == 't')
                {
                    output.push_back('\t');
                }
                else if(escaped == 'n')
                {
                    output.push_back('\n');
                }
                else
                {
                    output.push_back(escaped);
                }
            }
            else
            {
                output.push_back(advance());
            }
        }
        advance(); // eat ending quote
        return output;
    }

    void eatComment()
    {
        while(true)
        {
            char c = advance();
            if(c == '\n' || c == '\0') break;
        }
    }

    void skipWhitespace()
    {
        while(isspace(peek()) && !isDone())
        {
            advance();
        }
    }

    char peek() const
    {
        if(isDone()) return '\0';
        return *start;
    }

    char advance()
    {
        if(isDone()) return '\0';

        char t = *start;
        start++;
        if(t == '\n')
        {
            line += 1;
            col = 1;
        }
        else
        {
            col += 1;
        }
        return t;
    }

    bool isDone() const
    {
        return start == end;
    }

    bool isToken(char token) const
    {
        return token == '+' || token == '-' || token == '*' || token == '/' || token == '(' ||
            token == ')' || token == '=' || token == ';' || token == ':' || token == ',';
    }

    int line = 1;
    int col = 1;

    iter start;
    iter end;

    LexemeList lexemes;
};

// ----- public functions -----

LexerError::LexerError(std::string error, int line, int col)
    : std::runtime_error(error), line(line), col(col)
{}

bool operator ==(const Lexeme &a, const Lexeme &b)
{
    return
        a.name == b.name &&
        a.colPosition == b.colPosition &&
        a.lineNumber == b.lineNumber;
}

LexemeList Lexer::lexString(const std::string &sourceCode)
{
    return LexemeImpl(sourceCode.begin(), sourceCode.end()).lexemes;
}

LexemeList Lexer::lexFile(const std::string &filePath)
{
    // TODO: use iterators here too.
    std::ifstream t(filePath);
    std::stringstream buffer;
    buffer << t.rdbuf();

    return lexString(buffer.str());
}
