#ifndef INSTALLER_H
#define INSTALLER_H 1

#include "../parser/buildfile.h"

int install_priority(Buildfile* buildfile, const char* prefix, const char* sysroot, const char* argv_0);
int install(Buildfile* buildfile, const char* prefix, const char* argv_0);

#endif