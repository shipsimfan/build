#include "installer.h"

#define _GNU_SOURCE

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../config.h"

int install_file(const char* source_name, const char* target_name) {
    FILE* source = fopen(source_name, "r");
    if (source == NULL) {
        int err = errno;
        fprintf(stderr, "Error while opening %s: %s\n", source_name, strerror(err));
        return -1;
    }

    FILE* target = fopen(target_name, "w");
    if (target == NULL) {
        int err = errno;
        fprintf(stderr, "Error while opening %s: %s\n", target_name, strerror(err));
        fclose(source);
        return -1;
    }

    while (!feof(source) && !ferror(source) && !ferror(target))
        fputc(fgetc(source), target);

    if (ferror(source)) {
        fprintf(stderr, "Error while reading %s\n", source_name);
        fclose(source);
        fclose(target);
        return -1;
    }

    fclose(source);

    if (ferror(target)) {
        fprintf(stderr, "Error while writing %s\n", target_name);
        fclose(target);
        return -1;
    }

    fclose(target);

    return 0;
}

int install_directory(const char* source_path, const char* target_path) {
    DIR* directory = opendir(source_path);
    if (directory == NULL) {
        int err = errno;
        fprintf(stderr, "Error while opening %s: %s\n", source_path, strerror(err));
        return -1;
    }

    int source_length = strlen(source_path);
    int target_length = strlen(target_path);

    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;

        char* source = malloc(source_length + 1 + strlen(entry->d_name) + 1);
        sprintf(source, "%s/%s", source_path, entry->d_name);

        char* target = malloc(target_length + 1 + strlen(entry->d_name) + 1);
        sprintf(target, "%s/%s", target_path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            struct stat st;
            if (stat(target, &st) < 0)
                mkdir(target, 0775);

            if (install_directory(source, target) < 0) {
                free(source);
                free(target);
                return -1;
            }
        } else {
            if (install_file(source, target) < 0) {
                free(source);
                free(target);
                return -1;
            }
        }

        free(source);
        free(target);
    }

    return 0;
}

int install_priority(Buildfile* buildfile, const char* prefix, const char* sysroot, const char* argv_0) {
    if (buildfile->type != BUILDFILE_TYPE_GROUP || buildfile->priority->queue_length == 0)
        return 0;

    // Create command
    char* command;

    if (prefix) {
        if (sysroot) {
            command = malloc(strlen(argv_0) + 19 + strlen(prefix) + 11 + strlen(sysroot));
            sprintf(command, "%s install --prefix %s --sysroot %s", argv_0, prefix, sysroot);
        } else {
            command = malloc(strlen(argv_0) + 19 + strlen(prefix));
            sprintf(command, "%s install --prefix %s", argv_0, prefix);
        }
    } else {
        if (sysroot) {
            command = malloc(strlen(argv_0) + 20 + strlen(sysroot));
            sprintf(command, "%s install --sysroot %s", argv_0, sysroot);
        } else {
            command = malloc(strlen(argv_0) + 9);
            sprintf(command, "%s install", argv_0);
        }
    }

    for (int i = 0; i < buildfile->priority->queue_length; i++) {
        printf("Installing %s . . .\n", buildfile->priority->queue[i]);

        // Execute build on the sub-directory
        chdir(buildfile->priority->queue[i]);
        int status = system(command);
        chdir("..");

        if (status != EXIT_SUCCESS) {
            fprintf(stderr, "Error while installing sub-project '%s'\n", buildfile->priority->queue[i]);
            free(command);
            return -1;
        }

        putchar('\n');
    }

    free(command);
    return 0;
}

int install(Buildfile* buildfile, const char* prefix, const char* argv_0) {
    if (buildfile->type == BUILDFILE_TYPE_GROUP) {
        // Execute "build install" on subfolders
        // Open current directory
        DIR* directory = opendir(".");
        if (directory == NULL) {
            int err = errno;
            fprintf(stderr, "Error while opening current directory: %s\n", strerror(err));
            return -1;
        }

        // Create command
        char* command;

        if (prefix) {
            command = malloc(strlen(argv_0) + 30 + strlen(prefix));
            sprintf(command, "%s install --no-build --prefix %s", argv_0, prefix);
        } else {
            command = malloc(strlen(argv_0) + 20);
            sprintf(command, "%s install --no-build", argv_0);
        }

        // Loop through sub-directories
        struct dirent* entry;
        while ((entry = readdir(directory)) != NULL) {
            if (entry->d_type == DT_DIR) {
                // Ignore '.', '..', and any hidden folders
                if (entry->d_name[0] == '.')
                    continue;

                int done = 0;
                for (int i = 0; i < buildfile->priority->queue_length; i++) {
                    if (strcmp(buildfile->priority->queue[i], entry->d_name) == 0) {
                        done = 1;
                        break;
                    }
                }

                if (done)
                    continue;

                printf("Installing %s . . .\n", entry->d_name);

                // Execute build on the sub-directory
                chdir(entry->d_name);
                int status = system(command);
                chdir("..");

                if (status != EXIT_SUCCESS) {
                    fprintf(stderr, "Error while installing sub-project '%s'\n", entry->d_name);
                    free(command);
                    closedir(directory);
                    return -1;
                }
            }
        }

        free(command);
        closedir(directory);
    } else {
        // Install target
        char* source_name = generate_target_name(buildfile);
        if (prefix == NULL)
            prefix = DEFAULT_PREFIX;

        char* target_name = malloc(strlen(prefix) + 5 + strlen(source_name) + 1);
        sprintf(target_name, "%s/%s/%s", prefix, buildfile->type == BUILDFILE_TYPE_LIBRARY ? "lib" : "bin", source_name);

        if (install_file(source_name, target_name) < 0) {
            free(target_name);
            free(source_name);
            return -1;
        }

        free(target_name);
        free(source_name);

        if (buildfile->type == BUILDFILE_TYPE_LIBRARY) {
            // Install include directory
            char* include_target = malloc(strlen(prefix) + 10);
            sprintf(include_target, "%s/include/", prefix);
            if (install_directory("./include/", include_target) < 0) {
                free(include_target);
                return -1;
            }

            free(include_target);

            // Install objects
            for (int i = 0; i < buildfile->objects->buffer_length; i++) {
                char* object_name = malloc(strlen(prefix) + 5 + strlen(buildfile->objects->buffer[i]->target) + 1);
                sprintf(object_name, "%s/%s/%s", prefix, "lib", buildfile->objects->buffer[i]->target);

                if (install_file(buildfile->objects->buffer[i]->target, object_name) < 0) {
                    free(object_name);
                    return -1;
                }

                free(object_name);
            }
        }
    }

    return 0;
}