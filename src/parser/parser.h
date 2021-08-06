#ifndef PARSER_H
#define PARSER_H 1

#include "buildfile.h"

Buildfile* parse_buildfile(int verbose, const char* path);

#endif