#pragma once
#include <vector>
#include <string>

enum TokenType {
    TOKEN_EOF,              // End of file
    TOKEN_IDENTIFIER,       // Identifier
    TOKEN_INT_LITERAL,      // Integer literal
    TOKEN_PAREN_OPEN,       // (
    TOKEN_PAREN_CLOSE,      // )
    TOKEN_BRACKET_OPEN,     // {
    TOKEN_BRACKET_CLOSE,    // }
    TOKEN_ARROW,            // =>
    TOKEN_SEMICOLON,        // ;
    TOKEN_LET,              // let
    TOKEN_TYPE_INT,         // int
    TOKEN_COLON,            // :
    TOKEN_EQUAL,            // =
    TOKEN_LESSER,           // <
    TOKEN_BEGGER,           // >
    TOKEN_EQ_LESSER,        // >=
    TOKEN_EQ_BEGGER,        // <=
    TOKEN_EQ_EQ,            // ==
    TOKEN_NEQ,              // !=
    TOKEN_NOT,              // !
    TOKEN_BIT_AND,          // &
    TOKEN_BIT_OR,           // |
    TOKEN_BIT_XOR,          // ^
    TOKEN_BIT_COMP,         // ~
    TOKEN_AND,              // &&
    TOKEN_OR,               // ||
    TOKEN_ADD,              // +
    TOKEN_SUB,              // -
    TOKEN_MLPT,             // *
    TOKEN_DIV,              // /
    TOKEN_BIT_SHIFT_RIGHT,  // >>
    TOKEN_BIT_SHIFT_LEFT,   // <<
    TOKEN_MOD,              // %
    TOKEN_EQ_ADD,           // +=
    TOKEN_EQ_SUB,           // -=
    TOKEN_EQ_MLPT,          // *=
    TOKEN_EQ_DIV,           // /=
    TOKEN_COMA              //
};

struct Token {
    TokenType type;
    unsigned int line;
    union {
        char* identifier;
        long long intValue;
    }; 
};

struct Lexer {
private:
    char const* m_source;
    unsigned long long m_currentToken;
    unsigned int m_currentLine;
    std::vector<Token> m_tokens;
    char m_c;  
    
    bool isAlpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); 
    }

    bool isWhite(char c) {
        return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\f' || c == '\b';
    }

    bool isNumeric(char c) {
        return c >= '0' && c <= '9';
    }

    void next() {
        m_c = *(++m_source);
        
        if (m_c == '\n') {
            m_currentLine++;
        }
    }

    void prev() {
        if (m_c == '\n') {
            m_currentLine--;
        }

        m_c = *(--m_source);
    }

public:
    void analize(char const* source) {
        m_source = source;
        m_currentToken = 0;
        m_currentLine = 1;
        m_tokens.reserve(1024);

        m_c = *source;
        if (m_c == '\n') {
            m_currentLine++;
        }
        
        while (true) {
            while (isWhite(m_c)) {
                next();
            }

            switch (m_c) {
            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
            case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
            case 's': case 't': case 'u': case 'v': case 'w': case 'x':
            case 'y': case 'z': case 'A': case 'B': case 'C': case 'D':
            case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
            case 'K': case 'L': case 'M': case 'N': case 'O': case 'P':
            case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V':
            case 'W': case 'X': case 'Y': case 'Z': case '_':
            {
                std::string identifier;

                do {
                    identifier += m_c;
                    next();
                }
                while (isAlpha(m_c) || isNumeric(m_c) || m_c == '_');
                prev();

                Token token;
                token.type = TOKEN_IDENTIFIER;
                token.line = m_currentLine;
                token.identifier = (char*)malloc(identifier.size() + 1);
                identifier.copy(token.identifier, identifier.size());
                token.identifier[identifier.size()] = '\0';

                if (identifier == "let") {
                    token.type = TOKEN_LET;
                }
                else if (identifier == "int") {
                    token.type = TOKEN_TYPE_INT;
                }

                m_tokens.emplace_back(token);
                break;
            }
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            {
                std::string identifier;

                do {
                    identifier += m_c;
                    next();
                }
                while (isNumeric(m_c));
                prev();

                char* end;
                Token token;
                token.type = TOKEN_INT_LITERAL;
                token.line = m_currentLine;
                token.intValue = strtoll(identifier.c_str(), &end, 10);

                if (errno == ERANGE) {
                    printf("line %u, integer literal out of range", m_currentLine);
                    exit(1);
                }

                m_tokens.emplace_back(token);

                break;
            }
            case '=':
                next();
                if (m_c == '>') {
                    m_tokens.push_back(Token{ TOKEN_ARROW, m_currentLine });
                    break;
                }
                m_tokens.push_back(Token{ TOKEN_EQUAL, m_currentLine });
                break;
            case '+':
                next();
                if (m_c == '=') {
                    m_tokens.push_back(Token{ TOKEN_EQ_ADD, m_currentLine });
                    break;
                }
                prev();
                m_tokens.push_back(Token{ TOKEN_ADD, m_currentLine });
                break;
            case '-':
                next();
                if (m_c == '=') {
                    m_tokens.push_back(Token{ TOKEN_EQ_SUB, m_currentLine });
                    break;
                }
                prev();
                m_tokens.push_back(Token{ TOKEN_SUB, m_currentLine });
                break;
            case '*':
                next();

                if (m_c == '=') {
                    m_tokens.push_back(Token{ TOKEN_EQ_MLPT, m_currentLine });
                    break;
                }
                
                prev();
                m_tokens.push_back(Token{ TOKEN_MLPT, m_currentLine });
                break;
            case '/':
                next();

                if (m_c == '=') {
                    m_tokens.push_back(Token{ TOKEN_EQ_DIV, m_currentLine });
                    break;
                }
                
                prev();
                m_tokens.push_back(Token{ TOKEN_DIV, m_currentLine });
                break;
            case '&':
                next();

                if (m_c == '&') {
                    m_tokens.push_back(Token{ TOKEN_AND, m_currentLine });
                    break;
                }

                prev();
                m_tokens.push_back(Token{ TOKEN_BIT_AND, m_currentLine });
                break;
            case '|':
                next();

                if (m_c == '|') {
                    m_tokens.push_back(Token{ TOKEN_BIT_OR, m_currentLine });
                    break;
                }

                prev();
                m_tokens.push_back(Token{ TOKEN_OR, m_currentLine });
                break;
            case '%':
                m_tokens.push_back(Token{ TOKEN_MOD, m_currentLine });
                break;
            case '^':
                m_tokens.push_back(Token{ TOKEN_BIT_XOR, m_currentLine });
                break;
            case '~':
                next();
                m_tokens.push_back(Token{ TOKEN_BIT_COMP, m_currentLine });
                break;
            case '<':
                next();
                
                if (m_c == '=') {
                    m_tokens.push_back(Token{ TOKEN_EQ_LESSER, m_currentLine });
                    break;
                }
                
                if (m_c == '<') {
                    m_tokens.push_back(Token{ TOKEN_BIT_SHIFT_LEFT, m_currentLine });
                    break;
                }

                prev();
                m_tokens.push_back(Token{ TOKEN_LESSER, m_currentLine });
                break;
            case '>':
                next();
                
                if (m_c == '=') {
                    m_tokens.push_back(Token{ TOKEN_EQ_BEGGER, m_currentLine });
                    break;
                }

                if (m_c == '>') {
                    m_tokens.push_back(Token{ TOKEN_BIT_SHIFT_RIGHT, m_currentLine });
                    break;
                }

                prev();
                m_tokens.push_back(Token{ TOKEN_BEGGER, m_currentLine });
                break;
            case '!':
                next();
                
                if (m_c == '=') {
                    m_tokens.push_back(Token{ TOKEN_NEQ, m_currentLine });
                    break;
                }

                prev();
                m_tokens.push_back(Token{ TOKEN_NOT, m_currentLine });
                break;
            case ',':
                m_tokens.push_back(Token{ TOKEN_COMA, m_currentLine });
                break;
            case ';':
                m_tokens.push_back(Token{ TOKEN_SEMICOLON, m_currentLine });
                break;
            case ':':
                m_tokens.push_back(Token{ TOKEN_COLON, m_currentLine });
                break;
            case '(':
                m_tokens.push_back(Token{ TOKEN_PAREN_OPEN, m_currentLine });
                break;
            case ')':
                m_tokens.push_back(Token{ TOKEN_PAREN_CLOSE, m_currentLine });
                break;
            case '{':
                m_tokens.push_back(Token{ TOKEN_BRACKET_OPEN, m_currentLine });
                break;
            case '}':
                m_tokens.push_back(Token{ TOKEN_BRACKET_CLOSE, m_currentLine });
                break;
            case '\0':
                m_tokens.push_back(Token{ TOKEN_EOF, m_currentLine });
                return;
            default:
                printf("Unrecoginzed token on line %u, %c", m_currentLine, m_c);
                exit(EXIT_FAILURE);
            }

            next();
        }
    }

    Token& currentToken() {
        return m_tokens[m_currentToken];
    }

    Token& nextToken() {
        return m_tokens[m_currentToken++];
    }

    std::vector<Token> const& getTokens() {
        return m_tokens;
    }
};