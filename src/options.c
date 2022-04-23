#include "options.h"

#include <string.h>

static void sink(char* argv[], int32_t start, int32_t end) {
    char* arg;
    int32_t i;

    arg = argv[start];

    for (i = start; i > end; --i)
        argv[i] = argv[i - 1];

    argv[end] = arg;
}

static int32_t parse_opt(int32_t index, char* argv[], int32_t nopts,
                  struct option_t* opts) {
    int32_t i, type;

    type = (argv[index][1] == '-') ? 1 : 0;

    for (i = 0; i < nopts; ++i) {
        if (opts[i].active)
            continue;

        if (strncmp(argv[index] + 1 + type, opts[i].token[type], 256))
            continue;

        opts[i].active = 1;

        if (opts[i].flags & OPT_ARG)
            opts[i].string = argv[++index];

        return 1;
    }

    return 0;
}

int32_t parse_opts(int32_t argc, char* argv[], int32_t nopts,
                   struct option_t* opts) {
    int32_t i, count;

    count = 1;

    for (i = 1; i < argc; ++i) {
        if (argv[i][0] != '-' || argv[i][1] == '\0') {
            sink(argv, i, count++);
            continue;
        }

        if (!parse_opt(i, argv, nopts, opts))
            return -1;

        ++i;
    }

    return count;
}
