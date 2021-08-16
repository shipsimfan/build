#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../builder/builder.h"
#include "../cleaner/cleaner.h"
#include "../config.h"
#include "../installer/installer.h"
#include "../parser/parser.h"
#include "../util.h"
#include "options.h"

int cargo_build(const char* sysroot) {
    if (sysroot == NULL) {
        if (system("cargo build --target=x86_64-los") != EXIT_SUCCESS) {
            fprintf(stderr, "Error while building\n");
            return -1;
        }
    } else {
        char* command = malloc(21 + strlen(sysroot) + 38);
        sprintf(command,
                "RUSTFLAGS='--sysroot=%s/los' cargo build "
                "--target=x86_64-los",
                sysroot);

        printf("Command: %s\n", command);
        if (system(command) != EXIT_SUCCESS) {
            fprintf(stderr, "Error while building\n");
            free(command);
            return -1;
        }
    }

    return 0;
}

int main(int argc, const char* argv[]) {
    // Parse arguments
    Options* options = parse_arguments(argv);
    if (options == NULL)
        exit(EXIT_FAILURE);

    if (options->prefix == NULL)
        options->prefix = copy_string(DEFAULT_PREFIX);

    // Check for buildfile
    if (access(BUILDFILE_FILEPATH, F_OK) == 0) {
        // Parse buildfile
        Buildfile* buildfile =
            parse_buildfile(options->verbose, BUILDFILE_FILEPATH);
        if (buildfile == NULL) {
            destroy_options(options);
            exit(EXIT_FAILURE);
        }

        // Perform command
        int status;
        switch (options->command) {
        case COMMAND_BUILD:
            status = build_priority(buildfile, options->sysroot, argv[0]);
            if (status < 0)
                break;
            status = build(buildfile, options->sysroot, argv[0]);
            break;

        case COMMAND_CLEAN:
            status = clean(buildfile, argv[0]);
            break;

        case COMMAND_INSTALL:
            status = install_priority(buildfile, options->prefix,
                                      options->sysroot, argv[0]);
            if (status < 0)
                break;

            if (options->no_build == 0) {
                status = build(buildfile, options->sysroot, argv[0]);
                if (status != 0)
                    break;
            }
            status = install(buildfile, options->prefix, argv[0]);
            break;

        case COMMAND_INVALID:
            fprintf(stderr, "Attempting to use invalid command!\n");
            destroy_options(options);
            destroy_buildfile(buildfile);
            exit(EXIT_FAILURE);
        }

        if (status != 0) {
            destroy_options(options);
            destroy_buildfile(buildfile);
            exit(EXIT_FAILURE);
        }

        // Cleanup
        destroy_buildfile(buildfile);
    } else if (access("Cargo.toml", F_OK) == 0) {
        // Run cargo
        switch (options->command) {
        case COMMAND_CLEAN:
            if (system("cargo clean") != EXIT_SUCCESS) {
                fprintf(stderr, "Error while cleaning\n");
                destroy_options(options);
                exit(EXIT_FAILURE);
            }
            break;

        case COMMAND_BUILD:
            if (cargo_build(options->sysroot) < 0) {
                destroy_options(options);
                exit(EXIT_FAILURE);
            }
            break;

        case COMMAND_INSTALL: {
            if (options->no_build == 0) {
                if (cargo_build(options->sysroot) < 0) {
                    destroy_options(options);
                    exit(EXIT_FAILURE);
                }
            }

            char* command = malloc(35 + strlen(options->prefix) + 6);
            sprintf(command, "cp ./target/x86_64-los/debug/*.app %s/bin/",
                    options->prefix);
            if (system(command) != EXIT_SUCCESS) {
                fprintf(stderr, "Error while installing\n");
                destroy_options(options);
                exit(EXIT_FAILURE);
            }
            break;
        }

        case COMMAND_INVALID:
            fprintf(stderr, "Attempting to use invalid command!\n");
            destroy_options(options);
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "No buildfile or Cargo.toml located\n");
        destroy_options(options);
        exit(EXIT_SUCCESS);
    }

    destroy_options(options);
    exit(EXIT_SUCCESS);
}