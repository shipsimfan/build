#include "util.h"

#include <stdlib.h>
#include <string.h>

char* copy_path(const char* src) {
    int length = strlen(src);
    int trailing_slash = src[length - 1] == '/' || src[length - 1] == '\\';

    char* new = (char*)malloc(length + (trailing_slash ? 1 : 2));
    strcpy(new, src);
    if (!trailing_slash)
        strcat(new, "/");

    return new;
}

char* copy_string(const char* src) {
    int length = strlen(src);
    char* new_string = malloc(length + 1);
    strcpy(new_string, src);
    return new_string;
}