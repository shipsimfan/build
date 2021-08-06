#include "installer.h"

int install(Buildfile* buildfile, const char* prefix) {
    if (buildfile->type == BUILDFILE_TYPE_GROUP) {
        // Execute "build install" on subfolders
    } else {
        // Install target

        if (buildfile->type == BUILDFILE_TYPE_LIBRARY) {
            // Install include directory

            // Install objects
        }
    }

    return 0;
}