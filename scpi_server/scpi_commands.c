// scpi_commands.c : "*IDN?", "scopech1?" and "TRIG:LEV <val>"
#include "scpi_commands.h"
#include "rp.h"

#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// ====== Config hardware ======
#define BASE            0x40100000u
#define REG_ID          0x00
#define REG_STATUS      0x04
#define REG_DATA        0x08
#define REG_TRIG_LEVEL  0x0C   

// STATUS bits: [2]=capturing, [1]=fifo_empty, [0]=data_valid
#define ST_VALID    (1u << 0)
#define ST_EMPTY    (1u << 1)

#define N_SAMPLES   10000u   // 10k samples

// ====== helpers ======
static int send_line(int fd, const char *s) {
    if (write(fd, s, strlen(s)) < 0) return -1;
    if (write(fd, "\r\n", 2) < 0)   return -1;
    return 0;
}

static inline int16_t tc14_to_s16(uint32_t w) {
    w &= 0x3FFFu;                                   // 14 bits 
    return (w & 0x2000u) ? (int16_t)(w | 0xC000u)   // sign-extend -> 16b
                         : (int16_t)w;
}

static inline uint32_t pack_tc14_signed(int32_t v) {
    if (v >  8191) v =  8191;   // +0x1FFF
    if (v < -8192) v = -8192;   // -0x2000
    return (uint32_t)(v & 0x3FFF);
}

static void trim(char *s) {
    size_t n = strlen(s);
    while (n && (s[n-1]=='\n' || s[n-1]=='\r' || s[n-1]==' ' || s[n-1]=='\t')) s[--n] = 0;
    size_t i = 0;
    while (s[i]==' ' || s[i]=='\t') i++;
    if (i) memmove(s, s+i, n - i + 1);
}

static int ensure_init(void) {
    static int inited = 0;
    if (!inited) {
        if (rp_init(BASE) != 0) {
            return -1;
        }
        (void)rp_read(REG_ID); // ping
        inited = 1;
    }
    return 0;
}

// ====== impl ======
int scpi_handle_line(int fd, const char *line_in, size_t len) {
    char cmd[256];
    if (len >= sizeof(cmd)) len = sizeof(cmd) - 1;
    memcpy(cmd, line_in, len);
    cmd[len] = '\0';
    trim(cmd);

    // Identification
    if (strcmp(cmd, "*IDN?") == 0) {
        return send_line(fd, "REDPITAYA GEN1 OS2 Ponsin Leo ver.");
    }

    // --- TRIG:LEV <val> ---
    // <val> can be:
    //  - hex 14 bits: 0x51FF 
    //  - signed decimal   : -800 
    if (strncmp(cmd, "TRIG:LEV", 8) == 0) {
    const char *p = cmd + 8;
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '\0') {
        return send_line(fd, "ERR:ARG");
    }

    if (ensure_init() != 0) {
        return send_line(fd, "ERR:INIT");
    }

    unsigned long val;
    if ((p[0]=='0') && (p[1]=='x' || p[1]=='X')) {
        // Hex input → write raw
        val = strtoul(p, NULL, 16);
    } else {
        // Decimal input → also write raw
        val = strtoul(p, NULL, 10);
    }

    rp_write(REG_TRIG_LEVEL, (uint32_t)val);
    return send_line(fd, "OK");
}


    // Acquisition
    if (strcmp(cmd, "scopech1?") == 0) {
        if (ensure_init() != 0) {
            return send_line(fd, "ERR:INIT");
        }

        // Stream "v0,v1,...,v9999\r\n"
        char out[64];
        unsigned count = 0;

        while (count < N_SAMPLES) {
            uint32_t st = (uint32_t)rp_read(REG_STATUS);

            if (st & ST_VALID) {
                uint32_t raw = (uint32_t)rp_read(REG_DATA) & 0x3FFFu;
                // *** CHANGE: flip sign for display only ***
                int16_t  val = (int16_t)(-tc14_to_s16(raw));

                int n = (count == 0)
                      ? snprintf(out, sizeof(out), "%d", (int)val)
                      : snprintf(out, sizeof(out), ",%d", (int)val);
                if (n <= 0) return -1;
                if (write(fd, out, (size_t)n) < 0) return -1;

                count++;
            } else {
                if (st & ST_EMPTY) usleep(50);
            }
        }

        if (write(fd, "\r\n", 2) < 0) return -1;
        return 0;
    }

    return send_line(fd, "ERR:UNKNOWN");
}
