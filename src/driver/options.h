#ifndef DRIVER_H
#define DRIVER_H 1

enum Command { Invalid, Build, Install, Clean };

typedef struct {
    enum Command command;
    char* prefix;
    char* sysroot;
    int verbose;
} Options;

Options* parse_arguments(const char* argv[]);
void* destroy_options(Options* options);

#endif