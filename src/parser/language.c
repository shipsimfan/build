#include "language.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* construct_command_language(Language language, const char* sysroot, const char* source, const char* target, int include) {
    const char* extension = NULL;
    for (const char* p = source; *p; p++)
        if (*p == '.')
            extension = p;

    if (extension == NULL)
        return NULL;

    switch (language) {
    case LANGUAGE_C:
        if (strcmp(extension, ".c") == 0) {
            int command_length = 41 + strlen(target) + 1 + strlen(source) + 1;
            if (sysroot)
                command_length += 11 + strlen(sysroot);
            if (include)
                command_length += 12;

            char* command = malloc(command_length);
            sprintf(command, "clang --target=x86_64-los -Wall -g -c -o %s %s", target, source);
            if (sysroot) {
                strcat(command, " --sysroot=");
                strcat(command, sysroot);
            }

            if (include)
                strcat(command, " -I./include");

            return command;
        }
        return NULL;
    case LANGUAGE_CPP:
        if (strcmp(extension, ".cpp") == 0) {
            int command_length = 43 + strlen(target) + 1 + strlen(source) + 1;
            if (sysroot)
                command_length += 11 + strlen(sysroot);
            if (include)
                command_length += 12;

            char* command = malloc(command_length);
            sprintf(command, "clang++ --target=x86_64-los -Wall -g -c -o %s %s", target, source);
            if (sysroot) {
                strcat(command, " --sysroot=");
                strcat(command, sysroot);
            }
            if (include)
                strcat(command, " -I./include");

            return command;
        }

        return NULL;

    case LANGUAGE_ASM:
        if (strcmp(extension, ".asm") == 0) {
            char* command = malloc(29 + strlen(target) + 1 + strlen(source) + 1);
            sprintf(command, "nasm -f elf64 -g -F dwarf -o %s %s", target, source);
            return command;
        }

        return NULL;
    default:
        return NULL;
    }
}

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
        languages->buffer = realloc(languages->buffer, languages->buffer_capacity * sizeof(Language));
    }

    languages->buffer[languages->buffer_length] = language;
    languages->buffer_length++;
}

char* construct_command(Languages* languages, const char* sysroot, const char* source, const char* target, int include) {
    if (languages == NULL)
        return NULL;

    for (int i = 0; i < languages->buffer_length; i++) {
        char* command = construct_command_language(languages->buffer[i], sysroot, source, target, include);
        if (command != NULL)
            return command;
    }

    return NULL;
}

void destroy_languages(Languages* languages) {
    if (languages == NULL)
        return;

    free(languages->buffer);
    free(languages);
}