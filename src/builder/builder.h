#ifndef BUILDER_H
#define BUILDER_H 1

#include "../parser/buildfile.h"

int build_priority(Buildfile* buildfile, const char* sysroot, const char* argv_0);
int build(Buildfile* buildfile, const char* sysroot, const char* argv_0);

#endif