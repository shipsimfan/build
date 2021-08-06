#include "buildfile.h"

#include <stdio.h>
#include <stdlib.h>

void display_buildfile_type(BuildfileType type) {
    switch (type) {
    case BUILDFILE_TYPE_EXECUTABLE:
        printf("Executable");
        break;

    case BUILDFILE_TYPE_LIBRARY:
        printf("Library");
        break;

    case BUILDFILE_TYPE_GROUP:
        printf("Group");
        break;

    case BUILDFILE_TYPE_INVALID:
        printf("Invalid");
        break;
    }
}

Buildfile* create_buildfile() {
    Buildfile* buildfile = malloc(sizeof(Buildfile));
    buildfile->type = BUILDFILE_TYPE_INVALID;
    buildfile->name = NULL;
    buildfile->languages = create_languages();
    buildfile->objects = create_objects();
    return buildfile;
}

void display_buildfile(Buildfile* buildfile) {
    printf("Buildfile\n");
    printf("------------------------------\n");

    printf("Type: ");
    display_buildfile_type(buildfile->type);
    printf("\n");

    printf("Name: %s\n", buildfile->name);

    if (buildfile->languages->buffer_length > 0) {
        printf("Languages: [");
        for (int i = 0; i < buildfile->languages->buffer_length; i++) {
            display_language(buildfile->languages->buffer[i]);
            if (i != buildfile->languages->buffer_length - 1)
                printf(", ");
        }

        printf("]\n");
    }

    if (buildfile->objects->buffer_length > 0) {
        printf("Objects: ");
        display_objects(buildfile->objects);
    }

    putchar('\n');
}

void destroy_buildfile(Buildfile* buildfile) {
    if (buildfile == NULL)
        return;

    free(buildfile->name);
    destroy_languages(buildfile->languages);
    destroy_objects(buildfile->objects);

    free(buildfile);
}
