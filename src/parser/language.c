#include "language.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* construct_command_language(Language language, const char* sysroot, const char* source, const char* target, const char* extension) {
    switch (language) {
    case LANGUAGE_C:
        if (strcmp(extension, ".c") == 0) {
            int command_length = 38 + strlen(target) + 1 + strlen(source) + 1;
            if (sysroot)
                command_length += 11 + strlen(sysroot);

            char* command = malloc(command_length);
            strcpy(command, "clang --target=x86_64-los -Wall -g -o ");
            strcat(command, target);
            strcat(command, " ");
            strcat(command, source);
            if (sysroot) {
                strcat(command, " --sysroot=");
                strcat(command, sysroot);
            }

            return command;
        }
        return NULL;
    case LANGUAGE_CPP:
        if (strcmp(extension, ".cpp") == 0) {
            int command_length = 40 + strlen(target) + 1 + strlen(source) + 1;
            if (sysroot)
                command_length += 11 + strlen(sysroot);

            char* command = malloc(command_length);
            strcpy(command, "clang++ --target=x86_64-los -Wall -g -o ");
            strcat(command, target);
            strcat(command, " ");
            strcat(command, source);
            if (sysroot) {
                strcat(command, " --sysroot=");
                strcat(command, sysroot);
            }

            return command;
        }

        return NULL;

    case LANGUAGE_ASM:
        if (strcmp(extension, ".asm") == 0) {
            int command_length = 29 + strlen(target) + 1 + strlen(source) + 1;

            char* command = malloc(command_length);
            strcpy(command, "nasm -f elf64 -g -F dward -o ");
            strcat(command, target);
            strcat(command, " ");
            strcat(command, source);

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
        languages->buffer = realloc(
            languages->buffer, languages->buffer_capacity * sizeof(Language));
    }

    languages->buffer[languages->buffer_length] = language;
    languages->buffer_length++;
}

char* construct_command(Languages* languages, const char* sysroot, const char* source, const char* target) {
    if (languages == NULL)
        return NULL;

    const char* extension = NULL;
    for (const char* p = source; *p; p++)
        if (*p == '.')
            extension = p;

    for (int i = 0; i < languages->buffer_length; i++) {
        char* command = construct_command_language(languages->buffer[i], sysroot, source, target, extension);
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