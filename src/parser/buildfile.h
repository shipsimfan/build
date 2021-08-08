#ifndef BUILDFILE_H
#define BUILDFILE_H

#include "object.h"

typedef enum BuildfileType_t {
    BUILDFILE_TYPE_INVALID,
    BUILDFILE_TYPE_EXECUTABLE,
    BUILDFILE_TYPE_LIBRARY,
    BUILDFILE_TYPE_GROUP,
} BuildfileType;

typedef struct {
    BuildfileType type;
    char* name;
    Languages* languages;
    Objects* objects;
} Buildfile;

Buildfile* create_buildfile();
void display_buildfile(Buildfile* buildfile);
char* generate_target_name(Buildfile* buildfile);
void destroy_buildfile(Buildfile* buildfile);

#endif