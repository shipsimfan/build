#ifndef TOKEN_H
#define TOKEN_H 1

#include <stdio.h>

typedef enum TokenType_t {
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_NEWLINE,
    TOKEN_TYPE_EQUALS,
    TOKEN_TYPE_OPEN_BRACE,
    TOKEN_TYPE_CLOSE_BRACE,
    TOKEN_TYPE_COMMA,
    TOKEN_TYPE_END_OF_FILE,
} TokenType;

typedef struct {
    TokenType type;
    int line;
    int column;
    char* string;
} Token;

Token* create_simple_token(TokenType type, int line, int column);
Token* create_string_token(char* str, int line, int column);
void display_token(Token* token, FILE* stream);
void destroy_token(Token* token);

#endif