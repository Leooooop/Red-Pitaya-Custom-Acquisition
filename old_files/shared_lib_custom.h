#ifndef CUSTOM_SCOPE_LIB_H
#define CUSTOM_SCOPE_LIB_H

#include <stdint.h>
#include <stddef.h>   // size_t

#ifdef __cplusplus
extern "C" {
#endif

// 1) Initialise le mapping registres (utilise ta lib rp.c)
//    base_addr = base de ton custom_scope (ex: 0x40100000)
//    Retour 0 si OK, <0 si erreur.
int cs_init(uint32_t base_addr);

// 2) Ferme /dev/mem (à la fin)
void cs_close(void);

// 3) Lit 'count' échantillons en int16 (two's complement).
//    Si ob_to_tc=1, convertit Offset-Binary -> Two's-Complement.
//    Ecrit dans 'dst' (déjà alloué par l'appelant).
//    Place dans *got le nombre réellement lus (ici on bloque jusqu’à tout lire).
//    Retour 0 si OK.
int cs_read_block_i16(uint32_t count, int ob_to_tc, int16_t *dst, uint32_t *got);

// 4) Lit 'count' échantillons puis construit un BLOC SCPI binaire (#...<payload>).
//    Alloue *block (à free() par toi), *block_len = taille totale à envoyer.
//    Payload en big-endian (réseau).
//    Retour 0 si OK.
int cs_read_block_scpi(uint32_t count, int ob_to_tc, char **block, size_t *block_len);

#ifdef __cplusplus
}
#endif
#endif
