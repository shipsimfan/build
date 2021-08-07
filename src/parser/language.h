#ifndef LANGUAGES_H
#define LANGUAGES_H 1

typedef enum Language_t {
    LANGUAGE_INVALID,
    LANGUAGE_C,
    LANGUAGE_CPP,
    LANGUAGE_ASM,
} Language;

typedef struct {
    Language* buffer;
    int buffer_capacity;
    int buffer_length;
} Languages;

void display_language(Language language);

Languages* create_languages();
void push_languages(Languages* languages, Language language);
char* construct_command(Languages* languages, const char* sysroot, const char* source, const char* target);
void destroy_languages(Languages* languages);

#endif