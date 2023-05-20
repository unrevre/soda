#include "messages.h"

#include "soda.h"

#include "daemon.h"
#include "modules.h"

#include <stdint.h>
#include <string.h>
#include <unistd.h>

void handle_message(struct soda_t* soda, const char* buffer, int32_t len) {
    if (!strncmp(buffer, "empty", 6))
        return terminate(soda->opts);

    if (!strncmp(buffer, "note:", 5)) {
        if (buffer[5] == '\0')
            return;

        update_module(&soda->modules[NOTE], (void*)&buffer[5]);
        soda->modules[NOTE].active = 1;

        sleep(12);

        soda->modules[NOTE].active = 0;

        return;
    }
}
