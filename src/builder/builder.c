#include "builder.h"

int build(Buildfile* buildfile, const char* sysroot) {
    if (buildfile->type == BUILDFILE_TYPE_GROUP) {
        // Execute "build build" on subfolders
    } else {
        // Build source files

        if (buildfile->type == BUILDFILE_TYPE_LIBRARY) {
            // Link objects as static library

            // Build objects
        } else {
            // Link objects as executable
        }
    }

    return 0;
}