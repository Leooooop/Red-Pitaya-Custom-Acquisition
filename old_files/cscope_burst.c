#include "custom_scope_lib.h"
#include "rp.h"   // TA lib mmap: rp_init / rp_read / rp_cleanup

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>   // htons
#include <errno.h>

// --- Registres côté HDL (custom_scope) ---
#define REG_ID      0x00
#define REG_STATUS  0x04
#define REG_DATA    0x08

// STATUS bits
#define ST_VALID    (1u << 0)
#define ST_EMPTY    (1u << 1)

// État global tout bête
static int      g_is_init = 0;
static uint32_t g_base    = 0;

// OB14 -> TC14 (XOR du MSB)
static inline uint16_t ob14_to_tc14(uint16_t w) {
    return (uint16_t)(w ^ 0x2000u);
}

// TC14 -> int16 signé (extension du signe)
static inline int16_t tc14_to_s16(uint16_t w_tc) {
    w_tc &= 0x3FFFu;
    if (w_tc & 0x2000u) w_tc |= 0xC000u;
    return (int16_t)w_tc;
}

int cs_init(uint32_t base_addr) {
    if (g_is_init) return 0;
    if (rp_init((off_t)base_addr) != 0) {
        perror("rp_init");
        return -1;
    }
    g_base    = base_addr;
    g_is_init = 1;
    (void)rp_read(REG_ID); // ping (optionnel)
    return 0;
}

void cs_close(void) {
    if (!g_is_init) return;
    rp_cleanup();
    g_is_init = 0;
    g_base    = 0;
}

int cs_read_block_i16(uint32_t count, int ob_to_tc, int16_t *dst, uint32_t *got) {
    if (!g_is_init || !dst || !got) return -EINVAL;

    uint32_t n = 0;
    while (n < count) {
        uint32_t st = (uint32_t)rp_read(REG_STATUS);

        if (st & ST_VALID) {
            uint32_t raw = (uint32_t)rp_read(REG_DATA); // [13:0]
            uint16_t s14 = (uint16_t)(raw & 0x3FFFu);
            if (ob_to_tc) s14 = ob14_to_tc14(s14);
            dst[n++] = tc14_to_s16(s14);
        } else {
            if (st & ST_EMPTY) {
                // Laisse respirer le CPU quand FIFO vide
                usleep(50);
            }
        }
    }
    *got = n;
    return 0;
}

// Construit le header SCPI "#<ndigits><len>"
static int make_scpi_header(size_t payload_len, char **out, size_t *out_len) {
    char len_txt[32];
    int nd = snprintf(len_txt, sizeof(len_txt), "%zu", payload_len);
    if (nd <= 0) return -EINVAL;

    *out_len = (size_t)(2 + nd);
    *out = (char*)malloc(*out_len);
    if (!*out) return -ENOMEM;

    (*out)[0] = '#';
    (*out)[1] = (char)('0' + nd);
    memcpy(*out + 2, len_txt, (size_t)nd);
    return 0;
}

int cs_read_block_scpi(uint32_t count, int ob_to_tc, char **block, size_t *block_len) {
    if (!g_is_init || !block || !block_len) return -EINVAL;

    // 1) Lire les samples en RAM
    int16_t *tmp = (int16_t*)malloc(sizeof(int16_t) * count);
    if (!tmp) return -ENOMEM;

    uint32_t got = 0;
    int rc = cs_read_block_i16(count, ob_to_tc, tmp, &got);
    if (rc) { free(tmp); return rc; }

    // 2) Construire header SCPI
    size_t payload_len = (size_t)got * 2; // int16 -> 2 octets
    char *hdr = NULL; size_t hdr_len = 0;
    rc = make_scpi_header(payload_len, &hdr, &hdr_len);
    if (rc) { free(tmp); return rc; }

    // 3) Allouer bloc complet = header + payload
    *block_len = hdr_len + payload_len;
    *block = (char*)malloc(*block_len);
    if (!*block) { free(tmp); free(hdr); return -ENOMEM; }

    // 4) Copier header
    memcpy(*block, hdr, hdr_len);
    free(hdr);

    // 5) Copier payload en big-endian
    char *p = *block + hdr_len;
    for (uint32_t i = 0; i < got; ++i) {
        uint16_t u  = (uint16_t)tmp[i];
        uint16_t be = htons(u);            // big-endian
        memcpy(p + 2*i, &be, 2);
    }

    free(tmp);
    return 0;
}
