#ifndef _MAPPY_NUKLEAR_INST_H_
#define _MAPPY_NUKLEAR_INST_H_

#include "nuklear.h"
#include "input.h"

#include "engine/npc.h"
#include "engine/door.h"

#define NK_VERT_BUF_MAX (512 * 1024)
#define NK_ELEM_BUF_MAX (128 * 1024)

#define NPC_NAME_BUF_STATE_INACTIVE 2
#define NPC_NAME_BUF_STATE_ACTIVE 1

extern struct nk_context *nkctx;
extern struct nk_colorf tile_selected_colf;
extern struct nk_font *jbm_font;
extern int npc_name_buf_state;
extern npc_t npc_selected;
extern door_t door_selected;

/* base */
void nuklear_inst_init(void);
void nuklear_inst_render(const char *outpath, const int mouse[2],
			 const float dt);
void nuklear_inst_terminate(void);

/* panels */
void nuklear_inst_panel_project(const char *outpath, const float dt);
void nuklear_inst_panel_tile_selected(const int mouse_tile[2]);
void nuklear_inst_panel_npc(void);

#endif /* _MAPPY_NUKLEAR_INST_H_ */
