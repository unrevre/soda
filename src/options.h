#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdint.h>

#define OPT_ARG     0x1

struct option_t {
    char* token[2];
    char* string;
    int32_t active;
    int32_t flags;
};

int32_t parse_opts(int32_t argc, char* argv[], int32_t nopts,
                   struct option_t* opts);

#endif /* OPTIONS_H */
