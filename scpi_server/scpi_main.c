#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "scpi_commands.h"

#define LISTEN_PORT 5010
#define BACKLOG     8
#define RXBUF_SIZE  4096

static volatile int g_running = 1;
static void on_sigint(int s){ (void)s; g_running = 0; }

static int handle_client(int fd) {
    char buf[RXBUF_SIZE];
    char line[8192];
    size_t used = 0;

    while (g_running) {
        ssize_t n = recv(fd, buf, sizeof(buf), 0);
        if (n == 0) break;               // client fermé
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("recv");
            return -1;
        }

        // accumulate & parse by CRLF
        for (ssize_t i = 0; i < n; ++i) {
            if (used < sizeof(line)-1) line[used++] = buf[i];

            // check CRLF
            if (used >= 2 && line[used-2] == '\r' && line[used-1] == '\n') {
                // strip CRLF
                size_t len = used - 2;
                line[len] = '\0';
                scpi_handle_line(fd, line, len);
                used = 0;
            }
        }
    }
    return 0;
}

int main(void) {
    signal(SIGINT,  on_sigint);
    signal(SIGTERM, on_sigint);

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); return 1; }

    int yes = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(LISTEN_PORT);

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(s); return 1;
    }
    if (listen(s, BACKLOG) < 0) {
        perror("listen"); close(s); return 1;
    }

    printf("SCPI server listening on port %d …\n", LISTEN_PORT);

    while (g_running) {
        struct sockaddr_in cli; socklen_t clilen = sizeof(cli);
        int c = accept(s, (struct sockaddr*)&cli, &clilen);
        if (c < 0) {
            if (errno == EINTR) continue;
            perror("accept"); break;
        }
        handle_client(c);
        close(c);
    }

    close(s);
    printf("SCPI server stopped.\n");
    return 0;
}
