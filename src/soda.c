#include "soda.h"

#include "daemon.h"
#include "events.h"
#include "modules.h"
#include "options.h"
#include "window.h"
#include "worker.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct option_t* set_options(void) {
    int32_t size;
    struct option_t* opts;

    size = sizeof(struct option_t);
    opts = malloc(NOPTS * size);

    opts[OPT_DAEM] = (struct option_t) {
        { "d", "" },        "/dev/null",        0, 0 };
    opts[OPT_TERM] = (struct option_t) {
        { "t", "" },        "/dev/null",        0, 0 };
    opts[OPT_LOG] = (struct option_t) {
        { "l", "log" },     "/dev/null",        0, OPT_ARG };
    opts[OPT_PID] = (struct option_t) {
        { "p", "pid" },     "/tmp/pid-soda",    0, OPT_ARG };
    opts[OPT_SOCK] = (struct option_t) {
        { "s", "socket" },  "/tmp/socket-soda", 0, OPT_ARG };
    opts[OPT_MSG] = (struct option_t) {
        { "m", "" },        "",                 0, OPT_ARG };

    return opts;
}

void handle_options(struct option_t* opts) {
    if (opts[OPT_DAEM].active)
        daemonise(opts);

    if (opts[OPT_TERM].active)
        terminate(opts);

    if (opts[OPT_MSG].active)
        message(opts);
}

int soda(struct option_t* opts) {
    struct soda_t* soda;

    soda = malloc(sizeof(struct soda_t));
    soda->opts = opts;

    init_window(soda);
    init_modules(soda->modules);

    if (send_worker(&soda->t_svr, &server_loop, soda))
        return 1;

    if (send_worker(&soda->t_mod, &module_loop, soda))
        return 1;

    soda->modules[DATE].active = 1;
    soda->modules[INFO].active = 1;

    if (wait_worker(&soda->t_svr))
        return 1;

    if (wait_worker(&soda->t_mod))
        return 1;

    return 0;
}

int main(int argc, char* argv[]) {
    struct option_t* opts;

    opts = set_options();

    if (parse_opts(argc, argv, NOPTS, opts) != 1) {
        printf("usage: %s\n", argv[0]);
        return 1;
    }

    handle_options(opts);

    return soda(opts);
}
