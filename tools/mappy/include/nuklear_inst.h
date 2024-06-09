#ifndef _MAPPY_NUKLEAR_INST_H_
#define _MAPPY_NUKLEAR_INST_H_

#include "nuklear.h"
#include "glwin.h"

#define NK_VERT_BUF_MAX (512 * 1024)
#define NK_ELEM_BUF_MAX (128 * 1024)

extern struct nk_context *nkctx;
extern struct nk_colorf tile_selected_colf;

void nuklear_inst_init(void);
void nuklear_inst_render(const glwin_input_t *inp, const char *outpath,
			 const int mx_tile, const int my_tile);
void nuklear_inst_terminate(void);

#endif /* _MAPPY_NUKLEAR_INST_H_ */
