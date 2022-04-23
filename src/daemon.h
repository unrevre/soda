#ifndef DAEMON_H
#define DAEMON_H

#include "options.h"

void daemonise(struct option_t* opts);
void terminate(struct option_t* opts);
void message(struct option_t* opts);

#endif /* DAEMON_H */
