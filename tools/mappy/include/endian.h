#ifndef _MAPPY_ENDIAN_H_
#define _MAPPY_ENDIAN_H_

#include <stdio.h>
#include <stdint.h>

void fwrite_ef16(const uint16_t *ptr, FILE *file);
void fread_ef16(uint16_t *ptr, FILE *file);

#endif /* _MAPPY_ENDIAN_H_ */
