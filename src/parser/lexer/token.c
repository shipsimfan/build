#include "token.h"

#include <stdlib.h>

Token* create_simple_token(TokenType type, int line, int column) {
    Token* token = malloc(sizeof(Token));
    token->type = type;
    token->string = NULL;
    token->line = line;
    token->column = column;
    return token;
}

Token* create_string_token(char* string, int line, int column) {
    Token* token = malloc(sizeof(Token));
    token->type = TOKEN_TYPE_STRING;
    token->string = string;
    token->line = line;
    token->column = column;
    return token;
}

void display_token(Token* token, FILE* stream) {
    if (token == NULL)
        return;

    fprintf(stream, "%i:%i: ", token->line, token->column);

    switch (token->type) {
    case TOKEN_TYPE_STRING:
        fprintf(stream, "String (\"%s\")", token->string);
        break;

    case TOKEN_TYPE_NEWLINE:
        fprintf(stream, "Newline");
        break;

    case TOKEN_TYPE_EQUALS:
        fprintf(stream, "Equals");
        break;

    case TOKEN_TYPE_OPEN_BRACE:
        fprintf(stream, "Open brace");
        break;

    case TOKEN_TYPE_CLOSE_BRACE:
        fprintf(stream, "Close brace");
        break;

    case TOKEN_TYPE_COMMA:
        fprintf(stream, "Comma");
        break;

    case TOKEN_TYPE_END_OF_FILE:
        fprintf(stream, "End of file");
        break;
    }
}

void destroy_token(Token* token) {
    if (token == NULL)
        return;

    free(token->string);
    free(token);
}
