#ifndef INSTALLER_H
#define INSTALLER_H 1

#include "../parser/buildfile.h"

int install(Buildfile* buildfile, const char* prefix, const char* argv_0);

#endif