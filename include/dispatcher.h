#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "mouse_config.h"

#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(arr[0])

// creates a new type named `cmd_func`
typedef void (*cmd_func)(int argc, char **argv, int mode);

typedef struct commands{
    const char *name;
    cmd_func func;
    const char *help;

    struct commands *subcommands;
    int subcount;
} command_t;

typedef void (*opt_func)(mouse_config *, int argc, char **argv);
//sub options
typedef struct {
    const char *name;
    opt_func func;
    int args; // how many this option consume args
} option;

void dispatch(command_t *commands, int cmdcount, int argc, char **argv, int mode);
void sub_dispatch(option *sub_opts, int count, mouse_config *opts, int argc, char **argv);

#endif