#include "lexer.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TokenChain* tokenize_buildfile(const char* path) {
    // Open the buildfile
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        int err = errno;
        fprintf(stderr, "Error while opening %s: %s\n", path, strerror(err));
        return NULL;
    }

    // Prepare variables
    TokenChain* token_chain = create_token_chain();
    int column = 1;
    int line = 1;

    // Main token loop
    while (1) {
        // Get next character
        int c = fgetc(file);
        if (c < 0)
            break;

        // Parse character
        if (c == '\n')
            push_token_chain(
                token_chain,
                create_simple_token(TOKEN_TYPE_NEWLINE, line, column));
        else if (c == '=')
            push_token_chain(token_chain, create_simple_token(TOKEN_TYPE_EQUALS,
                                                              line, column));
        else if (c == '{')
            push_token_chain(
                token_chain,
                create_simple_token(TOKEN_TYPE_OPEN_BRACE, line, column));
        else if (c == '}')
            push_token_chain(
                token_chain,
                create_simple_token(TOKEN_TYPE_CLOSE_BRACE, line, column));
        else if (c == ',')
            push_token_chain(token_chain, create_simple_token(TOKEN_TYPE_COMMA,
                                                              line, column));
        else if (isalpha(c)) {
            // Parse string
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

            push_token_chain(token_chain,
                             create_string_token(buffer, line, column));
            continue;
        } else if (!isspace(c)) {
            fprintf(stderr, "Unknown token '%c' at %i:%i\n", c, line, column);
            destroy_token_chain(token_chain);
            fclose(file);
            return NULL;
        }

        // Adjust column and line
        if (c == '\n') {
            column = 1;
            line++;
        } else
            column++;
    }

    // Check to see if there was an error
    int err = errno;
    if (ferror(file)) {
        fprintf(stderr, "Error while reading %s: %s\n", path, strerror(err));
        destroy_token_chain(token_chain);
        fclose(file);
        return NULL;
    }

    // Cleanup
    fclose(file);
    push_token_chain(token_chain,
                     create_simple_token(TOKEN_TYPE_NEWLINE, line, column));
    push_token_chain(token_chain,
                     create_simple_token(TOKEN_TYPE_END_OF_FILE, line, column));
    return token_chain;
}