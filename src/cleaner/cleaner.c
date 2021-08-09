#include "cleaner.h"

#define _GNU_SOURCE

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../config.h"

int clean_directory(const char* path) {
    // Open current directory
    DIR* directory = opendir(path);
    if (directory == NULL) {
        int err = errno;
        fprintf(stderr, "Error while opening %s directory: %s\n", path, strerror(err));
        return -1;
    }

    int path_length = strlen(path);

    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char* entry_path = malloc(path_length + 1 + strlen(entry->d_name) + 1);
        sprintf(entry_path, "%s/%s", path, entry->d_name);
        if (entry->d_type == DT_DIR) {
            if (clean_directory(entry_path) < 0) {
                free(entry_path);
                closedir(directory);
                return -1;
            }
        } else {
            remove(entry_path);
        }

        free(entry_path);
    }

    closedir(directory);

    remove(path);

    return 0;
}

int clean(Buildfile* buildfile, const char* argv_0) {
    if (buildfile->type == BUILDFILE_TYPE_GROUP) {
        // Open current directory
        DIR* directory = opendir(".");
        if (directory == NULL) {
            int err = errno;
            fprintf(stderr, "Error while opening current directory: %s\n", strerror(err));
            return -1;
        }

        // Create command
        char* command = malloc(strlen(argv_0) + 7);
        sprintf(command, "%s clean", argv_0);

        // Loop through sub-directories
        struct dirent* entry;
        while ((entry = readdir(directory)) != NULL) {
            if (entry->d_type == DT_DIR) {
                // Ignore '.', '..', and any hidden folders
                if (entry->d_name[0] == '.')
                    continue;

                printf("Cleaning %s . . .\n", entry->d_name);

                // Execute build on the sub-directory
                chdir(entry->d_name);
                int status = system(command);
                chdir("..");

                if (status != EXIT_SUCCESS) {
                    fprintf(stderr, "Error while cleaning sub-project '%s'\n", entry->d_name);
                    free(command);
                    closedir(directory);
                    return -1;
                }
            }
        }

        free(command);
        closedir(directory);
    } else {
        // Remove obj/ directory
        struct stat st;
        if (stat(OBJECT_DIRECTORY, &st) >= 0)
            if (clean_directory(OBJECT_DIRECTORY) < 0)
                return -1;

        // Remove target
        char* target_name = generate_target_name(buildfile);
        remove(target_name);
        free(target_name);

        // Remove object targets
        for (int i = 0; i < buildfile->objects->buffer_length; i++)
            remove(buildfile->objects->buffer[i]->target);
    }

    return 0;
}