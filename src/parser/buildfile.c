#include "parser.h"

#include <stdlib.h>

#include "lexer/lexer.h"

Buildfile* parse_buildfile(int verbose) {
    // Tokenize the buildfile
    TokenChain* tokens = tokenize_buildfile();
    if (tokens == NULL)
        return NULL;

    if (verbose)
        display_token_chain(tokens);

    // Parse the tokens

    // Cleanup
    destroy_token_chain(tokens);
    return NULL;
}

void destroy_object(Object* object) {
    if (object == NULL)
        return;

    free(object->target);
    free(object->source);
    free(object);
}

void destroy_objects(Objects objects) {
    for (int i = 0; i < objects.buffer_length; i++)
        destroy_object(objects.buffer[i]);

    free(objects.buffer);
}

void destroy_buildfile(Buildfile* buildfile) {
    if (buildfile == NULL)
        return;

    free(buildfile->name);
    destroy_objects(buildfile->objects);
    destroy_languages(buildfile->languages);

    free(buildfile);
}