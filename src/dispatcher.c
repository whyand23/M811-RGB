#include "dispatcher.h"

// sub dispatcher for optional option
void sub_dispatch(option *sub_opts, int count, mouse_config *opts, int argc, char **argv)
{
    int i = 0;
    // i for argc while j are for the subcommands matching
    while(i < argc) {
        int matched = 0;
        for(int j = 0; j < count; j++) {
            if(strcmp(argv[i], sub_opts[j].name) == 0) {
                sub_opts[j].func(opts, argc - (i + 1), &argv[i + 1]);
                int token_to_skip = 1; // Consume the subcommand itself (1 token)
                token_to_skip += sub_opts[j].args; // Consume its arguments (example: rgb has 1 arg)
                i += token_to_skip; // Move i forward by that many tokens
                matched = 1;
                break;
            }
        }

        if(!matched) {
            printf("Unknown Input: %s\n", argv[i]);
            i++;
        }
    }
    return;
}

void dispatch(command_t *commands, int cmdcount, int argc, char **argv, int mode) 
{
    if(argc <= 0) {
        fprintf(stderr, "No command given to the dispatcher\n");
        return;
    }

    for(int i = 0; i < cmdcount; i++) {
        if(strcmp(argv[0], commands[i].name) == 0) { 
           
            // going deeper into subcommands
            if(commands[i].subcount > 0) {
                return dispatch(commands[i].subcommands, commands[i].subcount, argc - 1, &argv[1], mode);
            }
            // return leaf command if no more subcommands found
            return commands[i].func(argc - 1, &argv[1], mode);
        }
    }

    // if here is ever reached it means commands were not found
    printf("Command not found %s\n", argv[0]);
}

