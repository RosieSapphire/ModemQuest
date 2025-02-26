#include "endian.h"

void fwrite_ef16(const u16 *ptr, FILE *file)
{
	u16 flip = (u16)(((*ptr & 0x00FF) << 8) | ((*ptr & 0xFF00) >> 8));

	fwrite(&flip, 2, 1, file);
}

void fread_ef16(u16 *ptr, FILE *file)
{
	fread(ptr, 2, 1, file);
	*ptr = (u16)(((*ptr & 0x00FF) << 8) | ((*ptr & 0xFF00) >> 8));
}
