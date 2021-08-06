#ifndef PARSER_H
#define PARSER_H 1

#include "../languages/languages.h"

enum BuildfileType {
    Executable,
    Library,
    Group,
};

typedef struct {
    char* target;
    char* source;
    enum Language language;
} Object;

typedef struct {
    Object** buffer;
    int buffer_capacity;
    int buffer_length;
} Objects;

typedef struct {
    enum BuildfileType type;
    char* name;
    Languages languages;
    Objects objects;
} Buildfile;

Buildfile* parse_buildfile(int verbose);
void destroy_buildfile(Buildfile* buildfile);

#endif