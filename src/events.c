#include "events.h"

#include "soda.h"

#include "messages.h"
#include "modules.h"
#include "options.h"
#include "window.h"
#include "worker.h"

#include <CoreGraphics/CGContext.h>
#include <CoreGraphics/CGError.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

extern CGError SLSDisableUpdate(int cid);
extern CGError SLSReenableUpdate(int cid);
extern CGError SLSOrderWindow(int cid, uint32_t wid, int mode, uint32_t rel);

void* module_loop(void* data) {
    struct worker_t* w;
    struct soda_t* soda;
    struct window_t* win;
    CGContextRef context;

    w = data;
    soda = w->data;
    win = soda->win;
    context = win->context;

    while (w->flag) {
        update_module(&soda->modules[DATE]);
        update_module(&soda->modules[INFO]);

        SLSDisableUpdate(soda->cid);
        SLSOrderWindow(soda->cid, win->id, 0, 0);

        CGContextClearRect(context, win->frame);

        render_module(&soda->modules[DATE], context, 40, 800);
        render_module(&soda->modules[INFO], context, 40, 80);

        CGContextFlush(context);

        SLSOrderWindow(soda->cid, win->id, 1, 0);
        SLSReenableUpdate(soda->cid);

        sleep(4);
    }

    return NULL;
}

void* server_loop(void* data) {
    struct worker_t* w;
    struct soda_t* soda;
    struct option_t* opts;

    char* p_soc;

    struct sockaddr_un addr;
    int server, fd;

    char buffer[1024];
    uint32_t len;

    w = data;
    soda = w->data;
    opts = soda->opts;

    p_soc = opts[OPT_SOCK].string;

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, p_soc, sizeof(addr.sun_path));

    unlink(p_soc);

    if ((server = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror(" [!] socket\n");
        exit(1);
    }

    if (bind(server, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror(" [!] bind\n");
        exit(1);
    }

    if (listen(server, 16) == -1) {
        perror(" [!] listen\n");
        exit(1);
    }

    while (w->flag) {
        fd = accept(server, NULL, NULL);

        if (fd == -1) {
            perror(" [!] accept\n");
            exit(1);
        }

        while (len = recv(fd, &buffer, 1024, 0), len > 0) {
            buffer[len] = '\0';

            handle_message(soda, buffer, len);
        }

        close(fd);
    }

    return NULL;
}
