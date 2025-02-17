#ifndef _MQME_SHADER_H_
#define _MQME_SHADER_H_

#include "types.h"

u32 shader_init_from_file(const char *vpath, const char *fpath);
void shader_free(const u32 id);

#endif /* _MQME_SHADER_H_ */
