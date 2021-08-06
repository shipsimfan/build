#ifndef LEXER_H
#define LEXER_H 1

enum TokenType {
    String,
    Newline,
    Equals,
    OpenBrace,
    CloseBrace,
    Comma,
    EndOfFile,
};

typedef struct {
    enum TokenType type;
    int line;
    int column;
    char* string;
} Token;

typedef struct TokenNode_t {
    Token token;
    struct TokenNode_t* next;
} TokenNode;

typedef struct {
    TokenNode* head;
    TokenNode* tail;
} TokenChain;

TokenChain* tokenize_buildfile();
void display_token_chain(TokenChain* token_chain);
void destroy_token_chain(TokenChain* token_chain);

#endif