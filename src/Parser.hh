#pragma once
#include "Lexer.hh"

enum AST {
    AST_UNKNOWN,
    AST_VAR_DECL,
    AST_INT_VALUE,
    AST_BIN_EXPR
};

struct NodeAst;

struct VarDeclAst {
    char* name;
    NodeAst* assignment;
};

struct BinaryExprAst {
    NodeAst* left;
    NodeAst* right;
    char op;
};

struct NodeAst {
    AST ast;
    union {
        BinaryExprAst binExpr;
        VarDeclAst varDecl;
        long long intValue;
    };
};

struct Parser {
    Lexer m_lexer;
    std::vector<NodeAst> m_body;
    Token* m_tok;

public:
    void initParser(const char* source) {
        m_lexer.analize(source);
    }

    void next() {
        m_tok = &m_lexer.nextToken();
    }

    NodeAst* parseBinExprRoot() {
        next();

        if (m_tok->type != TOKEN_INT_LITERAL) {
            printf("Expected a value for variable declaration on line \x1b[31m%u\033[0m", m_tok->line);
            exit(EXIT_FAILURE);
        }

        NodeAst* node = (NodeAst*)malloc(sizeof(NodeAst));
        node->ast = AST_INT_VALUE;
        node->intValue = m_tok->intValue;
        return node;
    }

    NodeAst* parseBinaryExpr() {
        NodeAst* node = (NodeAst*)malloc(sizeof(NodeAst));
        node->ast = AST_BIN_EXPR;
        node->binExpr.left = parseBinExprRoot();

        next();
        switch (m_tok->type) {
            case TOKEN_ADD:
                node->binExpr.op = '+';
                break;
            case TOKEN_SUB:
                node->binExpr.op = '-';
                break;
            case TOKEN_MLPT:
                node->binExpr.op = '*';
                break;
            case TOKEN_DIV:
                node->binExpr.op = '/';
                break;
            case TOKEN_SEMICOLON:
                node->binExpr.right = NULL;
                return node;
                break;
            default:
                printf("Expected a ';' on line \x1b[31m%u\033[0m", m_tok->line);
                exit(EXIT_FAILURE);
                break;
        };

        node->binExpr.right = parseBinaryExpr();
        return node;
    }

    void parseIdentifier() {
        char* identifier = m_tok->identifier;
        next();

        
        if (m_tok->type != TOKEN_COLON) {
            printf("Expected a type declaration on line \x1b[31m%u\033[0m", m_tok->line);
            exit(EXIT_FAILURE);
        } 
        next();

        if (m_tok->type == TOKEN_TYPE_INT) {
            next();
        }
        
        if (m_tok->type != TOKEN_EQUAL) {
            printf("Expected a '=' after variable declaration on line \x1b[31m%u\033[0m", m_tok->line);
            exit(EXIT_FAILURE);
        }

        NodeAst node;
        node.ast = AST_VAR_DECL;
        node.varDecl.name = identifier;
        node.varDecl.assignment = parseBinaryExpr();
        m_body.push_back(node);
    }

    void parseLet() {
        next();
        if (m_tok->type != TOKEN_IDENTIFIER) {
            printf("Identifier expected on line \x1b[31m%u\033[0m", m_tok->line);
            exit(EXIT_FAILURE);
        }        
        parseIdentifier();
    }

    void generateAst() {
        while(true) {
            next();

            switch (m_tok->type) {
            case TOKEN_EOF:
                return;
            case TOKEN_LET:
                parseLet();
                break;
            default:
                printf("Identifier expected on line \x1b[31m%u\033[0m", m_tok->line);
                exit(EXIT_FAILURE);
            }
        }
    }

    int interpretTree(NodeAst* node) {

        if (node->ast == AST_INT_VALUE) {
            return node->intValue;
        }
        else if (node->ast == AST_BIN_EXPR) {
            int left, right;

            if (node->binExpr.left)  left  = interpretTree(node->binExpr.left);
            if (node->binExpr.right) right = interpretTree(node->binExpr.right);   

            if (node->binExpr.left && !node->binExpr.right) return left;         
            if (!node->binExpr.left && node->binExpr.right) return right;      

            switch(node->binExpr.op) {
            case '+': return left + right;
            case '-': return left - right;
            case '*': return left * right;
            case '/': return left / right;
            default: 
            return 0;
            }
        }
        return 0;
    }

    void printAst() {
        for (auto& var : m_body) {
            printf("Var %s: %d\n", var.varDecl.name, interpretTree(var.varDecl.assignment));
        }
    }
};
