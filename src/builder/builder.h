#ifndef BUILDER_H
#define BUILDER_H 1

#include "../parser/buildfile.h"

int build(Buildfile* buildfile, const char* sysroot);

#endif