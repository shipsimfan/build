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
#include <time.h>
#include <unistd.h>

typedef struct {
    char** paths;
    int capacity;
    int length;
    int built_files;
} BuiltFiles;

struct stat st;

BuiltFiles* create_builtfiles() {
    BuiltFiles* files = malloc(sizeof(BuiltFiles));
    files->paths = malloc(sizeof(char*));
    files->capacity = 1;
    files->built_files = 0;
    files->length = 0;
    return files;
}

void push_built_files(BuiltFiles* dest, BuiltFiles* src) {
    if (dest == NULL || src == NULL)
        return;

    dest->capacity += src->capacity;
    dest->paths = realloc(dest->paths, dest->capacity * sizeof(char*));
    for (int i = 0; i < src->length; i++)
        dest->paths[i + dest->length] = src->paths[i];

    dest->built_files += src->built_files;
    dest->length += src->length;

    free(src->paths);
    free(src);
}

void push_built_file(BuiltFiles* dest, char* path, int build) {
    if (dest == NULL)
        return;

    if (dest->length == dest->capacity) {
        dest->capacity *= 2;
        dest->paths = realloc(dest->paths, dest->capacity * sizeof(char*));
    }

    dest->paths[dest->length] = path;
    dest->length++;
    if (build)
        dest->built_files++;
}

void destroy_builtfiles(BuiltFiles* files) {
    if (files == NULL)
        return;

    for (int i = 0; i < files->length; i++)
        free(files->paths[i]);

    free(files->paths);
    free(files);
}

BuiltFiles* build_directory(Buildfile* buildfile, const char* sysroot, const char* path) {
    // Create output directory
    int path_length = strlen(path);
    char* target_path = malloc(path_length + 1);
    strcpy(target_path, path);
    target_path[2] = 'o';
    target_path[3] = 'b';
    target_path[4] = 'j';

    if (stat(target_path, &st) == -1)
        mkdir(target_path, 0775);

    // Open directory
    DIR* directory = opendir(path);
    if (directory == NULL) {
        int err = errno;
        fprintf(stderr, "Error while opening %s: %s\n", path, strerror(err));
        free(target_path);
        return NULL;
    }

    struct dirent* entry;
    BuiltFiles* built_files = create_builtfiles();
    while ((entry = readdir(directory)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;

        if (entry->d_type == DT_DIR) {
            // Construct new path
            char* dir_path = malloc(path_length + 1 + strlen(entry->d_name) + 1);
            sprintf(dir_path, "%s/%s", path, entry->d_name);

            // Build sub-directory
            BuiltFiles* sub_built = build_directory(buildfile, sysroot, dir_path);
            if (sub_built == NULL) {
                free(dir_path);
                free(target_path);
                destroy_builtfiles(built_files);
                return NULL;
            }

            push_built_files(built_files, sub_built);

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
            sprintf(source, "%s/%s", path, entry->d_name);

            if (extension)
                entry->d_name[name_length - extension_length - 1] = 0;
            char* target = malloc(source_length);
            sprintf(target, "%s/%s.o", target_path, entry->d_name);

            // Check file times
            int build = 0;
            if (stat(target, &st) < 0)
                build = 1;
            else {
                time_t target_time = st.st_mtime;
                stat(source, &st);
                if (difftime(st.st_mtime, target_time) >= 0)
                    build = 1;
            }

            // Build file if needed
            char* command = construct_command(buildfile->languages, sysroot, source, target, buildfile->type == BUILDFILE_TYPE_LIBRARY);
            if (command != NULL) {
                push_built_file(built_files, target, build);
                if (build) {
                    printf("Compiling %s . . .\n", source);

                    if (system(command) != EXIT_SUCCESS) {
                        fprintf(stderr, "Error while building %s\n", source);
                        free(source);
                        free(command);
                        free(target_path);
                        destroy_builtfiles(built_files);
                        closedir(directory);
                        return NULL;
                    }
                }
            } else
                free(target);

            free(command);
            free(source);
        }
    }

    closedir(directory);
    free(target_path);
    return built_files;
}

int build_priority(Buildfile* buildfile, const char* sysroot, const char* argv_0) {
    if (buildfile->type != BUILDFILE_TYPE_GROUP || buildfile->priority->queue_length == 0)
        return 0;

    char* command;
    if (sysroot) {
        command = malloc(strlen(argv_0) + 17 + strlen(sysroot) + 1);
        sprintf(command, "%s build --sysroot %s", argv_0, sysroot);
    } else {
        command = malloc(strlen(argv_0) + 7);
        sprintf(command, "%s build", argv_0);
    }

    for (int i = 0; i < buildfile->priority->queue_length; i++) {
        printf("Building %s . . .\n", buildfile->priority->queue[i]);

        // Execute build on the sub-directory
        chdir(buildfile->priority->queue[i]);
        int status = system(command);
        chdir("..");

        if (status != EXIT_SUCCESS) {
            fprintf(stderr, "Error whild building sub-project '%s'\n", buildfile->priority->queue[i]);
            free(command);
            return -1;
        }

        putchar('\n');
    }

    free(command);
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
            command = malloc(strlen(argv_0) + 17 + strlen(sysroot) + 1);
            sprintf(command, "%s build --sysroot %s", argv_0, sysroot);
        } else {
            command = malloc(strlen(argv_0) + 7);
            sprintf(command, "%s build", argv_0);
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

                putchar('\n');
            }
        }

        free(command);
        closedir(directory);
    } else {
        // Build source files
        BuiltFiles* built_files = build_directory(buildfile, sysroot, SOURCE_DIRECTORY);
        if (built_files == NULL)
            return -1;

        if (built_files->built_files == 0) {
            destroy_builtfiles(built_files);
            return 0;
        }

        char* target_name = generate_target_name(buildfile);

        printf("Linking %s . . .\n", target_name);

        if (buildfile->type == BUILDFILE_TYPE_LIBRARY) {
            // Link objects as static library
            int command_length = 7 + strlen(target_name) + 1;
            for (int i = 0; i < built_files->length; i++)
                command_length += strlen(built_files->paths[i]) + 1;

            char* command = malloc(command_length);
            sprintf(command, "ar rcs %s", target_name);
            for (int i = 0; i < built_files->length; i++) {
                strcat(command, " ");
                strcat(command, built_files->paths[i]);
            }

            if (system(command) != EXIT_SUCCESS) {
                fprintf(stderr, "Error while linking %s\n", target_name);
                free(target_name);
                free(command);
                destroy_builtfiles(built_files);
                return -1;
            }

            free(command);

            // Build objects
            for (int i = 0; i < buildfile->objects->buffer_length; i++) {
                Object* object = buildfile->objects->buffer[i];
                char* command = construct_command_language(object->language, sysroot, object->source, object->target, buildfile->type == BUILDFILE_TYPE_LIBRARY);
                if (command == NULL) {
                    fprintf(stderr, "Invalid source file (%s) for building %s\n", object->source, object->target);
                    free(target_name);
                    destroy_builtfiles(built_files);
                    return -1;
                }

                printf("Compiling %s . . .\n", object->source);
                if (system(command) != EXIT_SUCCESS) {
                    fprintf(stderr, "Error while compiling %s\n", object->source);
                    free(target_name);
                    free(command);
                    destroy_builtfiles(built_files);
                    return -1;
                }
            }
        } else {
            // Link objects as executable
            int command_length = 29 + strlen(target_name) + 1;
            if (sysroot)
                command_length += 10 + strlen(sysroot) + 1;
            for (int i = 0; i < built_files->length; i++)
                command_length += strlen(built_files->paths[i]) + 1;

            char* command = malloc(command_length);
            sprintf(command, "clang --target=x86_64-los -o %s", target_name);
            if (sysroot) {
                strcat(command, " --sysroot=");
                strcat(command, sysroot);
            }
            for (int i = 0; i < built_files->length; i++) {
                strcat(command, " ");
                strcat(command, built_files->paths[i]);
            }

            if (system(command) != EXIT_SUCCESS) {
                fprintf(stderr, "Error while linking %s\n", target_name);
                free(target_name);
                free(command);
                destroy_builtfiles(built_files);
                return -1;
            }

            free(command);
        }

        free(target_name);
        destroy_builtfiles(built_files);
    }

    return 0;
}