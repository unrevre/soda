#ifndef SODA_H
#define SODA_H

#include "modules.h"
#include "worker.h"

enum OPTS {
    OPT_DAEM,
    OPT_TERM,
    OPT_PID,
    OPT_LOG,
    OPT_SOCK,
    OPT_MSG,
    NOPTS
};

struct soda_t {
    struct option_t* opts;

    struct worker_t t_svr;
    struct worker_t t_mod;

    int cid;
    struct window_t* win;

    struct module_t modules[NMOD];
};

#endif /* SODA_H */
