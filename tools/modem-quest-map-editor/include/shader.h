#ifndef MQME_SHADER_H
#define MQME_SHADER_H

#include "types.h"

u32 shader_init_from_file(const char *vpath, const char *fpath);
void shader_free(const u32 id);

#endif /* MQME_SHADER_H */
