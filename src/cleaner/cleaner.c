#include "cleaner.h"

int clean(Buildfile* buildfile) {
    if (buildfile->type == BUILDFILE_TYPE_GROUP) {
        // Execute "build clean" on subfolders
    } else {
        // Remove obj/ directory

        // Remove target

        // Remove object targets
    }

    return 0;
}