#include "parser.h"

#include <stdlib.h>
#include <string.h>

#include "../util.h"
#include "lexer/lexer.h"

#define POP_NEXT_TOKEN(ttype, str)                                             \
    token = pop_token_chain(token_chain);                                      \
    if (token == NULL || token->type != ttype) {                               \
        fprintf(stderr, "Error: Expected %s, instead found (", str);           \
        if (token == NULL)                                                     \
            fprintf(stderr, "Null");                                           \
        else                                                                   \
            display_token(token, stderr);                                      \
        fprintf(stderr, ")\n");                                                \
        destroy_token(token);                                                  \
        return -1;                                                             \
    }

#define POP_NEXT_TOKEN2(type1, type2, str)                                     \
    token = pop_token_chain(token_chain);                                      \
    if (token == NULL || (token->type != type1 && token->type != type2)) {     \
        fprintf(stderr, "Error: Expected %s, instead found (", str);           \
        if (token == NULL)                                                     \
            fprintf(stderr, "Null");                                           \
        else                                                                   \
            display_token(token, stderr);                                      \
        fprintf(stderr, ")\n");                                                \
        destroy_token(token);                                                  \
        return -1;                                                             \
    }

#define POP_NEXT_TOKEN3(type1, type2, type3, str)                              \
    token = pop_token_chain(token_chain);                                      \
    if (token == NULL || (token->type != type1 && token->type != type2 &&      \
                          token->type != type3)) {                             \
        fprintf(stderr, "Error: Expected %s, instead found (", str);           \
        if (token == NULL)                                                     \
            fprintf(stderr, "Null");                                           \
        else                                                                   \
            display_token(token, stderr);                                      \
        fprintf(stderr, ")\n");                                                \
        destroy_token(token);                                                  \
        return -1;                                                             \
    }

typedef enum DirectiveType_t {
    DIRECTIVE_INVALID,
    DIRECTIVE_TYPE,
    DIRECTIVE_NAME,
    DIRECTIVE_LANGUAGE,
    DIRECTIVE_OBJECTS,
} DirectiveType;

DirectiveType parse_directive_type(const char* str) {
    if (strcmp(str, "type") == 0)
        return DIRECTIVE_TYPE;
    if (strcmp(str, "name") == 0)
        return DIRECTIVE_NAME;
    if (strcmp(str, "language") == 0)
        return DIRECTIVE_LANGUAGE;
    if (strcmp(str, "objects") == 0)
        return DIRECTIVE_OBJECTS;

    return DIRECTIVE_INVALID;
}

int parse_name(TokenChain* token_chain, Buildfile* buildfile) {
    Token* POP_NEXT_TOKEN(TOKEN_TYPE_STRING, "name");

    free(buildfile->name);
    buildfile->name = copy_string(token->string);

    destroy_token(token);

    POP_NEXT_TOKEN(TOKEN_TYPE_NEWLINE, "newline");
    destroy_token(token);

    return 0;
}

int parse_type(TokenChain* token_chain, Buildfile* buildfile) {
    Token* POP_NEXT_TOKEN(TOKEN_TYPE_STRING, "type");

    if (strcmp(token->string, "executable") == 0)
        buildfile->type = BUILDFILE_TYPE_EXECUTABLE;
    else if (strcmp(token->string, "library") == 0)
        buildfile->type = BUILDFILE_TYPE_LIBRARY;
    else if (strcmp(token->string, "group") == 0)
        buildfile->type = BUILDFILE_TYPE_GROUP;
    else {
        fprintf(stderr, "Error: Invalid type '%s'\n", token->string);
        destroy_token(token);
        return -1;
    }

    destroy_token(token);
    POP_NEXT_TOKEN(TOKEN_TYPE_NEWLINE, "newline");
    destroy_token(token);

    return 0;
}

Language parse_language(const char* str) {
    if (strcmp(str, "c") == 0)
        return LANGUAGE_C;
    if (strcmp(str, "c++") == 0)
        return LANGUAGE_CPP;
    if (strcmp(str, "asm") == 0)
        return LANGUAGE_ASM;
    return LANGUAGE_INVALID;
}

int parse_languages(TokenChain* token_chain, Buildfile* buildfile) {
    Token* token;
    while (1) {
        POP_NEXT_TOKEN(TOKEN_TYPE_STRING, "language");

        Language language = parse_language(token->string);
        if (language == LANGUAGE_INVALID) {
            fprintf(stderr, "Error: Invalid language '%s'\n", token->string);
            destroy_token(token);
            return -1;
        }

        push_languages(buildfile->languages, language);

        destroy_token(token);
        POP_NEXT_TOKEN2(TOKEN_TYPE_COMMA, TOKEN_TYPE_NEWLINE, "comma");

        if (token->type == TOKEN_TYPE_NEWLINE) {
            destroy_token(token);
            return 0;
        }

        destroy_token(token);
    }
}

int parse_objects(TokenChain* token_chain, Buildfile* buildfile) {
    Token* POP_NEXT_TOKEN(TOKEN_TYPE_OPEN_BRACE, "open curly brace");

    destroy_token(token);
    while (1) {
        POP_NEXT_TOKEN3(TOKEN_TYPE_NEWLINE, TOKEN_TYPE_CLOSE_BRACE,
                        TOKEN_TYPE_STRING, "object name or close curly brace");

        if (token->type == TOKEN_TYPE_NEWLINE) {
            destroy_token(token);
            continue;
        }

        if (token->type == TOKEN_TYPE_CLOSE_BRACE) {
            destroy_token(token);
            POP_NEXT_TOKEN(TOKEN_TYPE_NEWLINE, "newline");
            destroy_token(token);
            return 0;
        }

        Object* object = create_object();
        object->target = copy_string(token->string);
        destroy_token(token);

        POP_NEXT_TOKEN(TOKEN_TYPE_EQUALS, "equals");
        destroy_token(token);
        POP_NEXT_TOKEN(TOKEN_TYPE_OPEN_BRACE, "open curly brace");
        destroy_token(token);
        while (1) {
            POP_NEXT_TOKEN3(TOKEN_TYPE_CLOSE_BRACE, TOKEN_TYPE_NEWLINE,
                            TOKEN_TYPE_STRING, "object directive");
            if (token->type == TOKEN_TYPE_CLOSE_BRACE) {
                destroy_token(token);
                break;
            }

            if (token->type == TOKEN_TYPE_NEWLINE) {
                destroy_token(token);
                continue;
            }

            int language;
            if (strcmp(token->string, "source") == 0)
                language = 0;
            else if (strcmp(token->string, "language") == 0)
                language = 1;
            else {
                fprintf(stderr, "Error: invalid object directive '%s'\n",
                        token->string);
                destroy_object(object);
                destroy_token(token);
                return -1;
            }

            destroy_token(token);

            POP_NEXT_TOKEN(TOKEN_TYPE_EQUALS, "equals");
            destroy_token(token);

            POP_NEXT_TOKEN(TOKEN_TYPE_STRING, language ? "language" : "source");

            if (language) {
                if (object->language != LANGUAGE_INVALID) {
                    fprintf(stderr,
                            "Attempting to set two languages on object %s\n",
                            object->target);
                    destroy_object(object);
                    destroy_token(token);
                    return -1;
                }

                object->language = parse_language(token->string);
                if (object->language == LANGUAGE_INVALID) {
                    fprintf(stderr, "Error: Invalid language '%s'\n",
                            token->string);
                    destroy_object(object);
                    destroy_token(token);
                    return -1;
                }
            } else {
                if (object->source != NULL) {
                    fprintf(stderr,
                            "Attempting to set two sources on object %s\n",
                            object->target);
                    destroy_object(object);
                    destroy_token(token);
                    return -1;
                }

                object->source = copy_string(token->string);
            }

            destroy_token(token);

            POP_NEXT_TOKEN(TOKEN_TYPE_NEWLINE, "newline");
            destroy_token(token);
        }

        if (object->source == NULL) {
            fprintf(stderr, "No source specified for object %s\n",
                    object->target);
            destroy_object(object);
            return -1;
        }

        if (object->language == LANGUAGE_INVALID) {
            fprintf(stderr, "No language specified for object %s\n",
                    object->target);
            destroy_object(object);
            return -1;
        }

        push_objects(buildfile->objects, object);

        POP_NEXT_TOKEN(TOKEN_TYPE_NEWLINE, "newline");
        destroy_token(token);
    }
}

int parse_directive(TokenChain* token_chain, Buildfile* buildfile) {
    Token* POP_NEXT_TOKEN2(TOKEN_TYPE_STRING, TOKEN_TYPE_NEWLINE,
                           "directive name");
    if (token->type == TOKEN_TYPE_NEWLINE) {
        destroy_token(token);
        return 0;
    }

    DirectiveType directive_type = parse_directive_type(token->string);
    if (directive_type == DIRECTIVE_INVALID) {
        fprintf(stderr, "Error: Invalid directive '%s'\n", token->string);
        destroy_token(token);
        return -1;
    }

    destroy_token(token);
    POP_NEXT_TOKEN(TOKEN_TYPE_EQUALS, "'='");
    destroy_token(token);

    switch (directive_type) {
    case DIRECTIVE_NAME:
        return parse_name(token_chain, buildfile);
    case DIRECTIVE_TYPE:
        return parse_type(token_chain, buildfile);
    case DIRECTIVE_LANGUAGE:
        return parse_languages(token_chain, buildfile);
    case DIRECTIVE_OBJECTS:
        return parse_objects(token_chain, buildfile);
    default:
        return -1;
    }
}

Buildfile* parse_buildfile(int verbose, const char* path) {
    // Tokenize the buildfile
    TokenChain* token_chain = tokenize_buildfile(path);
    if (token_chain == NULL)
        return NULL;

    if (verbose)
        display_token_chain(token_chain);

    // Parse the tokens
    Buildfile* buildfile = create_buildfile();
    while (1) {
        Token* token = pop_token_chain(token_chain);
        if (token->type == TOKEN_TYPE_END_OF_FILE)
            break;

        push_front_token_chain(token_chain, token);

        if (parse_directive(token_chain, buildfile) < 0) {
            destroy_token_chain(token_chain);
            destroy_buildfile(buildfile);
            return NULL;
        }
    }

    if (verbose)
        display_buildfile(buildfile);

    // Cleanup
    destroy_token_chain(token_chain);
    return buildfile;
}
