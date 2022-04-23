#include "messages.h"

#include "soda.h"

#include "daemon.h"

#include <stdint.h>
#include <string.h>

void handle_message(struct soda_t* soda, const char* buffer, int32_t len) {
    if (!strncmp(buffer, "empty", 6))
        return terminate(soda->opts);
}
