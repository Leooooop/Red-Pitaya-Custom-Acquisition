#include "shared_lib_custom.h"
#include "rp.h"         // tes rp_init/rp_read/rp_write/rp_cleanup
#include <unistd.h>
#include <stdio.h>

#define BASE        0x40100000u
#define REG_ID      0x00
#define REG_STATUS  0x04
#define REG_DATA    0x08
// STATUS: [1]=fifo_empty, [0]=data_valid
#define ST_VALID    (1u << 0)
#define ST_EMPTY    (1u << 1)

// OB14 -> TC14 si force_ob=1
static inline uint32_t ob14_to_tc14(uint32_t w) { return (w ^ 0x2000u) & 0x3FFFu; }
// garde 14 bits et étend le signe en 32 bits
static inline int32_t tc14_to_s32(uint32_t w) {
    w &= 0x3FFFu;
    if (w & 0x2000u) w |= 0xFFFFC000u;
    return (int32_t)w;
}

int cscope_init(void) {
    if (rp_init(BASE) != 0) {
        perror("rp_init");
        return -1;
    }
    (void)rp_read(REG_ID); // ping optionnel
    return 0;
}

void cscope_release(void) {
    rp_cleanup();
}

int cscope_read_burst(uint32_t max_samples, int force_ob,
                      int32_t *out, uint32_t *out_n)
{
    if (!out || !out_n) return -1;
    *out_n = 0;
    uint32_t n = 0;

    while (n < max_samples) {
        uint32_t st = rp_read(REG_STATUS);
        if (st & ST_VALID) {
            uint32_t raw14 = (uint32_t)rp_read(REG_DATA) & 0x3FFFu;
            uint32_t tc14  = force_ob ? ob14_to_tc14(raw14) : raw14;
            out[n++] = tc14_to_s32(tc14);
        } else {
            // si FIFO vide, souffle un peu pour le CPU
            if (st & ST_EMPTY) usleep(50);
        }
    }
    *out_n = n;
    return 0;
}
