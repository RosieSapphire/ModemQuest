#ifndef MQME_ENDIAN_H
#define MQME_ENDIAN_H

#include <stdio.h>

#include "types.h"

void fwrite_ef16(const u16 *ptr, FILE *file);
void fread_ef16(u16 *ptr, FILE *file);

#endif /* MQME_ENDIAN_H */
