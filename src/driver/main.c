#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../builder/builder.h"
#include "../cleaner/cleaner.h"
#include "../config.h"
#include "../installer/installer.h"
#include "../parser/parser.h"
#include "../util.h"
#include "options.h"

int main(int argc, const char* argv[]) {
    // Parse arguments
    Options* options = parse_arguments(argv);
    if (options == NULL)
        exit(EXIT_FAILURE);

    // Parse buildfile
    Buildfile* buildfile = parse_buildfile(options->verbose, BUILDFILE_FILEPATH);
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
        if (options->prefix == NULL)
            options->prefix = copy_string(DEFAULT_PREFIX);

        status = install_priority(buildfile, options->prefix, options->sysroot, argv[0]);
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
    destroy_options(options);
    destroy_buildfile(buildfile);
    exit(EXIT_SUCCESS);
}