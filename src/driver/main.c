#include <stdio.h>
#include <stdlib.h>

#include "../parser/parser.h"
#include "config.h"
#include "options.h"

int main(int argc, const char* argv[]) {
    // Parse arguments
    Options* options = parse_arguments(argv);
    if (options == NULL)
        exit(EXIT_FAILURE);

    // Parse buildfile
    Buildfile* buildfile =
        parse_buildfile(options->verbose, BUILDFILE_FILEPATH);
    if (buildfile == NULL) {
        destroy_options(options);
        exit(EXIT_FAILURE);
    }

    // Perform command

    // Cleanup
    destroy_options(options);
    destroy_buildfile(buildfile);
    exit(EXIT_SUCCESS);
}