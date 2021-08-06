#include "lexer.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../config.h"

Token new_simple_token(enum TokenType type, int line, int column) {
    Token token;
    token.type = type;
    token.string = NULL;
    token.line = line;
    token.column = column;
    return token;
}

Token new_string_token(char* string, int line, int column) {
    Token token;
    token.type = String;
    token.string = string;
    token.line = line;
    token.column = column;
    return token;
}

void insert_into_token_chain(TokenChain* token_chain, Token token) {
    TokenNode* new_node = (TokenNode*)malloc(sizeof(TokenNode));
    new_node->token = token;
    new_node->next = NULL;

    if (token_chain->head == NULL) {
        token_chain->head = new_node;
        token_chain->tail = new_node;
    } else {
        token_chain->tail->next = new_node;
        token_chain->tail = new_node;
    }
}

TokenChain* tokenize_buildfile() {
    FILE* file = fopen(BUILDFILE_FILEPATH, "r");
    if (file == NULL) {
        int err = errno;
        fprintf(stderr, "Error while opening %s: %s\n", BUILDFILE_FILEPATH,
                strerror(err));
        return NULL;
    }

    TokenChain* token_chain = (TokenChain*)malloc(sizeof(TokenChain));
    token_chain->head = NULL;
    token_chain->tail = NULL;

    int column = 1;
    int line = 1;
    while (1) {
        int c = fgetc(file);
        if (c < 0)
            break;

        if (c == '\n')
            insert_into_token_chain(token_chain,
                                    new_simple_token(Newline, line, column));
        else if (c == '=')
            insert_into_token_chain(token_chain,
                                    new_simple_token(Equals, line, column));
        else if (c == '{')
            insert_into_token_chain(token_chain,
                                    new_simple_token(OpenBrace, line, column));
        else if (c == '}')
            insert_into_token_chain(token_chain,
                                    new_simple_token(CloseBrace, line, column));
        else if (c == ',')
            insert_into_token_chain(token_chain,
                                    new_simple_token(Comma, line, column));
        else if (isalpha(c)) {
            char* buffer = malloc(1);
            int buffer_length = 1;
            int buffer_offset = 0;
            while (isalnum(c) || c == '.' || c == '_') {
                if (buffer_length == buffer_offset) {
                    buffer_length *= 2;
                    buffer = realloc(buffer, buffer_length);
                }

                buffer[buffer_offset] = c;
                buffer_offset++;

                c = fgetc(file);
            }

            ungetc(c, file);
            column += buffer_offset;

            if (buffer_length == buffer_offset)
                buffer = realloc(buffer, buffer_length + 1);

            buffer[buffer_offset] = 0;

            insert_into_token_chain(token_chain,
                                    new_string_token(buffer, line, column));
            continue;
        } else if (!isspace(c)) {
            fprintf(stderr, "Unknown token '%c' at %i:%i\n", c, line, column);
            destroy_token_chain(token_chain);
            fclose(file);
            return NULL;
        }

        if (c == '\n') {
            column = 1;
            line++;
        } else
            column++;
    }

    int err = errno;
    if (ferror(file)) {
        fprintf(stderr, "Error while reading %s: %s\n", BUILDFILE_FILEPATH,
                strerror(err));
        destroy_token_chain(token_chain);
        fclose(file);
        return NULL;
    }

    fclose(file);

    insert_into_token_chain(token_chain,
                            new_simple_token(EndOfFile, line, column));

    return token_chain;
}

void display_token(Token token) {
    printf("%i:%i: ", token.line, token.column);

    switch (token.type) {
    case String:
        printf("String (\"%s\")", token.string);
        break;

    case Newline:
        printf("Newline");
        break;

    case Equals:
        printf("Equals");
        break;

    case OpenBrace:
        printf("Open brace");
        break;

    case CloseBrace:
        printf("Close brace");
        break;

    case Comma:
        printf("Comma");
        break;

    case EndOfFile:
        printf("End of file");
        break;
    }

    putchar('\n');
}

void display_token_chain(TokenChain* token_chain) {
    if (token_chain == NULL)
        return;

    printf("Tokens\n");
    printf("------------------------------\n");
    TokenNode* node = token_chain->head;
    while (node != NULL) {
        display_token(node->token);
        node = node->next;
    }

    printf("\n");
}

void destroy_token(Token token) { free(token.string); }

void destroy_token_chain(TokenChain* token_chain) {
    if (token_chain == NULL)
        return;

    TokenNode* node = token_chain->head;
    while (node != NULL) {
        TokenNode* next = node->next;
        destroy_token(node->token);
        free(node);
        node = next;
    }

    free(token_chain);
}