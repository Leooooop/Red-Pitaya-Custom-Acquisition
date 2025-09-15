#include <sys/mman.h>

// fifo_read_file_burst.c — lecture simple + centrage DC optionnel
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "rp.h"
#include <string.h> 

#define BASE        0x40100000u
#define REG_ID      0x00
#define REG_STATUS  0x04
#define REG_DATA    0x08
// STATUS: [2]=capturing, [1]=fifo_empty, [0]=data_valid
#define ST_VALID    (1u << 0)
#define ST_EMPTY    (1u << 1)

#define DEFAULT_MAX_SAMPLES 10000u

static inline uint32_t to_tc14(uint32_t s14, int force_ob) {
    s14 &= 0x3FFFu;
    if (force_ob) s14 ^= 0x2000u;          // OB -> TC si demandé
    return s14;
}
static inline int32_t s14_to_s32(uint32_t s14_tc) {
    return (s14_tc & 0x2000u) ? (int32_t)(s14_tc | 0xFFFFC000u) : (int32_t)s14_tc;
}

int main(int argc, char **argv)
{
    // Args: [max_samples] [out.csv] [--ob] [--rm-dc]
    uint32_t max_samples = (argc > 1 && argv[1][0] != '-') ? (uint32_t)strtoul(argv[1], NULL, 0)
                                                           : DEFAULT_MAX_SAMPLES;
    const char *csv_path = (argc > 2 && argv[2][0] != '-') ? argv[2] : "data.csv";
    int force_ob = 0;     // suppose que le HDL sort déjà du TC14
    int remove_dc = 0;

    for (int i = 1; i < argc; ++i) {
        if      (!strcmp(argv[i], "--ob"))     force_ob = 1;
        else if (!strcmp(argv[i], "--rm-dc"))  remove_dc = 1;
    }

    if (rp_init(BASE) != 0) { perror("rp_init"); return 1; }
    (void)rp_read(REG_ID);

    // On met tout en RAM pour pouvoir retirer la moyenne ensuite
    int32_t *buf = (int32_t*)malloc(sizeof(int32_t) * max_samples);
    if (!buf) { perror("malloc"); rp_cleanup(); return 1; }

    uint32_t n = 0;
    int32_t vmin =  2147483647, vmax = -2147483647;
    long long sum = 0;

    while (n < max_samples) {
        // attendre qu’une donnée soit prête (VALID) ou pause si vide
        uint32_t st = rp_read(REG_STATUS);
        if (st & ST_VALID) {
            uint32_t raw  = rp_read(REG_DATA);            // [13:0] utile
            uint32_t s14  = to_tc14(raw & 0x3FFFu, force_ob);
            int32_t  val  = s14_to_s32(s14);

            buf[n++] = val;
            if (val < vmin) vmin = val;
            if (val > vmax) vmax = val;
            sum += val;
        } else {
            // FIFO vide -> laisse respirer le CPU
            if (st & ST_EMPTY) usleep(50);
        }
    }

    double mean = (n > 0) ? (double)sum / (double)n : 0.0;

    FILE *csv = fopen(csv_path, "w");
    if (!csv) { perror("fopen"); free(buf); rp_cleanup(); return 1; }
    fprintf(csv, "index,value_raw,value_centered\n");
    for (uint32_t i = 0; i < n; ++i) {
        double centered = remove_dc ? (buf[i] - mean) : buf[i];
        fprintf(csv, "%u,%d,%.0f\n", i, buf[i], centered);
    }
    fclose(csv);

    printf("Samples: %u  min=%d  max=%d  mean=%.2f  (OB->TC:%s  RM-DC:%s)\n",
           n, vmin, vmax, mean, force_ob ? "on" : "off", remove_dc ? "on" : "off");

    free(buf);
    rp_cleanup();
    return 0;
}
