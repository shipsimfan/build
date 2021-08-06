#include <stdio.h>
#include <stdlib.h>

#include "../parser/parser.h"
#include "options.h"

int main(int argc, const char* argv[]) {
    // Parse arguments
    Options* options = parse_arguments(argv);
    if (options == NULL) {
        fprintf(stderr, "Error while parsing arguments\n");
        exit(EXIT_FAILURE);
    }

    // Parse buildfile
    Buildfile* buildfile = parse_buildfile(options->verbose);
    if (buildfile == NULL) {
        fprintf(stderr, "Error while parsing buildfile\n");
        destroy_options(options);
        exit(EXIT_FAILURE);
    }

    // Perform command

    // Cleanup
    destroy_options(options);
    destroy_buildfile(buildfile);
    exit(EXIT_SUCCESS);
}