#ifndef SCPI_COMMANDS_H
#define SCPI_COMMANDS_H

#include <stddef.h>

// Traite UNE ligne (sans \r\n), envoie la réponse sur 'fd'
int scpi_handle_line(int fd, const char *line, size_t len);

#endif // SCPI_COMMANDS_H
