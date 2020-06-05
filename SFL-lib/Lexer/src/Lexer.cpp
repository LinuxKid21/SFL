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
        current = _start;
        end = _end;

        skipWhitespace();
        while(!isDone())
        {
            int wordLine = line;
            int wordCol = col;
            try
            {
                getNext();
            }
            catch(const std::exception& e)
            {
                // append location information to all errors
                throw LexerError(e.what(), line, col);
            }
            
        }
    }

    void getNext()
    {
        int wordLine = line;
        int wordCol = col;

        char c = peek();
        if(isWordChar(c) && !isdigit(c))
        {
            lexemes.push_back(Lexeme{getWord(), wordLine, wordCol, Lexeme::Type::Identifier});
        }
        else if(isdigit(c) || c == '.')
        {
            lexemes.push_back(Lexeme{getNumber(), wordLine, wordCol, Lexeme::Type::Number});
        }
        else if(isToken(c))
        {
            advance(); // eat the token

            std::string tokenString;
            if(c == '*' && peek() == '*')
            {
                advance();
                tokenString = "**";
            }
            else if(c == '=' && peek() == '=')
            {
                advance();
                tokenString = "==";
            }
            else
            {
                tokenString = std::string(1, c);
            }
            lexemes.push_back(Lexeme{tokenString, wordLine, wordCol, Lexeme::Type::Operator});
        }
        else if(c == '"')
        {
            lexemes.push_back(Lexeme{getString(), wordLine, wordCol, Lexeme::Type::String});
        }
        else if(c == '#')
        {
            // TODO: turn the comment into a token so we can associate documentation with things
            eatComment();
        }
        else
        {
            throw LexerError("Unexpected token");
        }
        
        skipWhitespace();
    }

    bool isWordChar(char c) const
    {
        return isalnum(c) || c == '_' || c == '?' || c == '!';
    }

    std::string getWord()
    {
        iter wordStart = current;
        while(isWordChar(peek()))
        {
            advance();
        }
        return std::string(wordStart, current);
    }

    std::string getNumber()
    {
        iter wordStart = current;
        bool foundDecimal = false;
        while(isdigit(peek()) || peek() == '.')
        {
            if(peek() == '.')
            {
                if(foundDecimal)
                {
                    throw LexerError("Two decimals in one number");
                }
                else
                {
                    foundDecimal = true;
                }
            }
            advance();
        }

        // the case where there is just the decimal
        if(foundDecimal && std::distance(wordStart, current) == 1)
        {
            throw LexerError("Unexpected token");
        }

        return std::string(wordStart, current);
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
        return *current;
    }

    char advance()
    {
        if(isDone()) return '\0';

        char t = *current;
        current++;
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
        return current == end;
    }

    bool isToken(char token) const
    {
        return token == '+' || token == '-' || token == '*' || token == '/' || token == '(' ||
            token == ')' || token == '=' || token == ';' || token == ':' || token == ',' || token == '.';
    }

    int line = 1;
    int col = 1;

    iter current;
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
