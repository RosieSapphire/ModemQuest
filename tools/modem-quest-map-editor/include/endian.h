#ifndef _MQME_ENDIAN_H_
#define _MQME_ENDIAN_H_

#include <stdio.h>

#include "types.h"

void fwrite_ef16(const u16 *ptr, FILE *file);
void fread_ef16(u16 *ptr, FILE *file);

#endif /* _MQME_ENDIAN_H_ */
