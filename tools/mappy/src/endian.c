#include "endian.h"

void fwrite_ef16(const uint16_t *ptr, FILE *file)
{
	uint16_t flip = ((*ptr & 0x00FF) << 8) | ((*ptr & 0xFF00) >> 8);

	fwrite(&flip, 2, 1, file);
}

void fread_ef16(uint16_t *ptr, FILE *file)
{
	fread(ptr, 2, 1, file);

	uint16_t flip = ((*ptr & 0x00FF) << 8) | ((*ptr & 0xFF00) >> 8);

	*ptr = flip;
}
