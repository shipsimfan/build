#ifndef LANGUAGES_H
#define LANGUAGES_H 1

enum Language {
    c,
    cpp,
    assembly,
};

typedef struct {
    enum Language* buffer;
    int buffer_capacity;
    int buffer_length;
} Languages;

void destroy_languages(Languages languages);

#endif