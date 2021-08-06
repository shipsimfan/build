#ifndef OPTIONS_H
#define OPTIONS_H 1

typedef enum Command_t {
    COMMAND_INVALID,
    COMMAND_BUILD,
    COMMAND_INSTALL,
    COMMAND_CLEAN
} Command;

typedef struct {
    Command command;
    char* prefix;
    char* sysroot;
    int verbose;
} Options;

Options* parse_arguments(const char* argv[]);
void* destroy_options(Options* options);

#endif