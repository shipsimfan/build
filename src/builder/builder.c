#include "builder.h"

#define _GNU_SOURCE

#include "../config.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct stat st;

int build_directory(Buildfile* buildfile, const char* sysroot, const char* path) {
    // Create output directory
    int path_length = strlen(path);
    char* target_path = malloc(path_length + 1);
    strcpy(target_path, path);
    target_path[2] = 'o';
    target_path[3] = 'b';
    target_path[4] = 'j';

    if (stat(target_path, &st) == -1)
        mkdir(target_path, 0600);

    // Open directory
    DIR* directory = opendir(path);
    if (directory == NULL) {
        int err = errno;
        fprintf(stderr, "Error while opening %s: %s\n", path, strerror(err));
        free(target_path);
        return -1;
    }

    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;

        if (entry->d_type == DT_DIR) {
            // Construct new path
            char* dir_path = malloc(path_length + 1 + strlen(entry->d_name) + 1);
            strcpy(dir_path, path);
            strcat(dir_path, "/");
            strcat(dir_path, entry->d_name);

            // Build sub-directory
            if (build_directory(buildfile, sysroot, dir_path) < 0) {
                free(dir_path);
                free(target_path);
                return -1;
            }

            free(dir_path);
        } else {
            // Construct paths
            int extension = 0;
            int extension_length = 0;
            for (char* p = entry->d_name; *p; p++) {
                if (*p == '.') {
                    extension = 1;
                    extension_length = 0;
                } else if (extension)
                    extension_length++;
            }

            int name_length = strlen(entry->d_name);
            int source_length = path_length + 1 + name_length + 1;
            int target_length = path_length + 1 + name_length - extension_length + 1 + 1;
            if (!extension)
                target_length++;

            char* source = malloc(source_length);
            strcat(source, path);
            strcat(source, "/");
            strcat(source, entry->d_name);

            if (extension)
                entry->d_name[name_length - extension_length - 1] = 0;
            char* target = malloc(source_length);
            strcat(target, target_path);
            strcat(target, "/");
            strcat(target, entry->d_name);
            strcat(target, ".o");

            char* command = construct_command(buildfile->languages, sysroot, source, target);
            free(target);
            if (command != NULL) {
                if (system(command) != EXIT_SUCCESS) {
                    fprintf(stderr, "Error while building %s\n", source);
                    free(source);
                    free(command);
                    free(target_path);
                    closedir(directory);
                    return -1;
                }

                free(command);
            }

            free(source);
        }
    }

    closedir(directory);
    free(target_path);
    return 0;
}

int build(Buildfile* buildfile, const char* sysroot, const char* argv_0) {
    if (buildfile->type == BUILDFILE_TYPE_GROUP) {
        // Open current directory
        DIR* directory = opendir(".");
        if (directory == NULL) {
            int err = errno;
            fprintf(stderr, "Error while opening current directory: %s\n", strerror(err));
            return -1;
        }

        // Create command
        char* command;
        if (sysroot) {
            int length = strlen(argv_0) + 17 + strlen(sysroot) + 1;
            command = malloc(length);
            strcpy(command, argv_0);
            strcat(command, " build --sysroot ");
            strcat(command, sysroot);
        } else {
            int length = strlen(argv_0) + 7;
            command = malloc(length);
            strcpy(command, argv_0);
            strcat(command, " build");
        }

        // Loop through sub-directories
        struct dirent* entry;
        while ((entry = readdir(directory)) != NULL) {
            if (entry->d_type == DT_DIR) {
                // Ignore '.', '..', and any hidden folders
                if (entry->d_name[0] == '.')
                    continue;

                printf("Building %s . . .\n", entry->d_name);

                // Execute build on the sub-directory
                chdir(entry->d_name);
                int status = system(command);
                chdir("..");

                if (status != EXIT_SUCCESS) {
                    fprintf(stderr, "Error whild building sub-project '%s'\n", entry->d_name);
                    free(command);
                    closedir(directory);
                    return -1;
                }
            }
        }

        free(command);
        closedir(directory);
    } else {
        // Build source files
        if (build_directory(buildfile, sysroot, SOURCE_DIRECTORY) < 0)
            return -1;

        if (buildfile->type == BUILDFILE_TYPE_LIBRARY) {
            // Link objects as static library

            // Build objects
        } else {
            // Link objects as executable
        }
    }

    return 0;
}