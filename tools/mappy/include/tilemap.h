#ifndef _MAPPY_TILEMAP_H_
#define _MAPPY_TILEMAP_H_

#include "types.h"
#include "glwin.h"

#include "game/tiles.h"

extern tile_t tilemap[TILES_H_MAX][TILES_W_MAX];
extern tile_t tile_selected;
extern u16 tilemap_w;
extern u16 tilemap_h;
extern int tilemap_pan_x, tilemap_pan_y, tilemap_is_zoomed;

void tilemap_load(const char *path);
void tilemap_init(void);
void tilemap_update(const glwin_input_t *inp, const int mx_tile,
		    const int my_tile, const char *outpath, const float dt);
void tilemap_render(const int mx_tile, const int my_tile);
void tilemap_resize(const int w_new, const int h_new);
void tilemap_get_mouse_tile_pos(const glwin_input_t *inp,
				int *mx_tile, int *my_tile);
int tilemap_is_mouse_inside(const glwin_input_t *inp, const int mx_tile,
			    const int my_tile);

#endif /* _MAPPY_TILEMAP_H_ */
