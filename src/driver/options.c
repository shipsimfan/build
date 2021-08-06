#include "options.h"

#include <stdio.h>
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

Options* create_default_options() {
    Options* options = (Options*)malloc(sizeof(Options));
    options->command = Build;
    options->prefix = NULL;
    options->sysroot = NULL;
    options->verbose = 0;
    return options;
}

enum Command parse_command(const char* str) {
    if (strcmp(str, "build") == 0)
        return Build;
    else if (strcmp(str, "install") == 0)
        return Install;
    else if (strcmp(str, "clean") == 0)
        return Clean;
    return Invalid;
}

void display_version() {
    printf("build version 0.1\n");
    exit(EXIT_SUCCESS);
}

Options* parse_arguments(const char* argv[]) {
    // Create default options
    Options* options = create_default_options();

    // Verify first argument
    if (argv[0] == NULL) {
        fprintf(stderr, "Invalid invocation\n");
        return destroy_options(options);
    }

    // Parse command
    if (argv[1] == NULL)
        return options;

    if (strcmp(argv[1], "--version") == 0) {
        destroy_options(options);
        display_version();
    }

    int i;
    if (argv[1][0] != '-') {
        i = 2;
        options->command = parse_command(argv[1]);
        if (options->command == Invalid) {
            fprintf(stderr, "Invalid command '%s'\n", argv[1]);
            return destroy_options(options);
        }
    } else
        i = 1;

    // Parse options
    for (; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--prefix") == 0) {
            i++;
            if (argv[i] == NULL) {
                fprintf(stderr, "No path after '-prefix'\n");
                return destroy_options(options);
            }

            if (options->prefix != NULL) {
                fprintf(stderr, "Attempting to set two prefixes\n");
                return destroy_options(options);
            }

            options->prefix = copy_path(argv[i]);
        } else if (strcmp(argv[i], "-s") == 0 ||
                   strcmp(argv[i], "--sysroot") == 0) {
            i++;
            if (argv[i] == NULL) {
                fprintf(stderr, "No path after '-sysroot'\n");
                return destroy_options(options);
            }

            if (options->sysroot != NULL) {
                fprintf(stderr, "Attempting to set two sysroots\n");
                return destroy_options(options);
            }

            options->sysroot = copy_path(argv[i]);
        } else if (strcmp(argv[i], "-v") == 0 ||
                   strcmp(argv[i], "--verbose") == 0)
            options->verbose = 1;
        else if (strcmp(argv[i], "--version") == 0) {
            destroy_options(options);
            display_version();
        } else {
            fprintf(stderr, "Unknown argument '%s'\n", argv[i]);
            return destroy_options(options);
        }
    }

    return options;
}

void* destroy_options(Options* options) {
    if (options == NULL)
        return NULL;

    free(options->prefix);
    free(options->sysroot);
    free(options);
    return NULL;
}