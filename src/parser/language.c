#include "language.h"

#include <stdio.h>
#include <stdlib.h>

void display_language(Language language) {
    switch (language) {
    case LANGUAGE_C:
        printf("c");
        break;
    case LANGUAGE_CPP:
        printf("c++");
        break;
    case LANGUAGE_ASM:
        printf("assembly");
        break;
    case LANGUAGE_INVALID:
        printf("invalid");
        break;
    }
}

Languages* create_languages() {
    Languages* languages = malloc(sizeof(Languages));
    languages->buffer = malloc(sizeof(Language));
    languages->buffer_capacity = 1;
    languages->buffer_length = 0;
    return languages;
}

void push_languages(Languages* languages, Language language) {
    if (languages == NULL || language == LANGUAGE_INVALID)
        return;

    if (languages->buffer_capacity == languages->buffer_length) {
        languages->buffer_capacity *= 2;
        languages->buffer = realloc(
            languages->buffer, languages->buffer_capacity * sizeof(Language));
    }

    languages->buffer[languages->buffer_length] = language;
    languages->buffer_length++;
}

void destroy_languages(Languages* languages) {
    if (languages == NULL)
        return;

    free(languages->buffer);
    free(languages);
}