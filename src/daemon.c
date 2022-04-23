#include "daemon.h"

#include "soda.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

void daemonise(struct option_t* opts) {
    FILE *f;
    pid_t pid;

    char* p_log;
    char* p_pid;

    p_log = opts[OPT_LOG].string;
    p_pid = opts[OPT_PID].string;

    switch (pid = fork()) {
        case -1:
            perror(" [!] fork\n");
            exit(1);
        case 0:
            freopen("/dev/null", "r", stdin);
            freopen(p_log, "w", stdout);
            freopen(p_log, "w", stderr);

            setsid();
            break;
        default:
            f = fopen(p_pid, "w");
            fprintf(f, "%d", pid);
            fclose(f);

            exit(0);
    }
}

void terminate(struct option_t* opts) {
    FILE *f;
    pid_t pid;

    char* p_pid;

    p_pid = opts[OPT_PID].string;

    if ((f = fopen(p_pid, "r"))) {
        fscanf(f, "%d", &pid);

        unlink(p_pid);

        sleep(4);

        kill(pid, SIGTERM);
    }

    exit(0);
}

void message(struct option_t* opts) {
    struct sockaddr_un addr;
    int fd;

    char* p_soc;
    char* p_msg;

    p_soc = opts[OPT_SOCK].string;
    p_msg = opts[OPT_MSG].string;

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, p_soc, sizeof(addr.sun_path));

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror(" [!] socket\n");
        exit(1);
    }

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror(" [!] connect\n");
        exit(1);
    }

    if (send(fd, p_msg, strnlen(p_msg, 1024), 0) == -1) {
        perror(" [!] send\n");
        exit(1);
    }

    close(fd);

    exit(0);
}
