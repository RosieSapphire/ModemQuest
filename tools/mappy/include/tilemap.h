#ifndef _MAPPY_TILEMAP_H_
#define _MAPPY_TILEMAP_H_

#include "types.h"
#include "glwin.h"

#define joypad_buttons_t void *
#include "game/tilemap.h"
#undef joypad_buttons_t

extern int tilemap_pan_x, tilemap_pan_y, tilemap_is_zoomed;
extern tile_t tile_selected;

/* base */
void tilemap_update(const glwin_input_t *inp, const int mx_tile,
		    const int my_tile, const float dt);
void tilemap_get_mouse_tile_pos(const glwin_input_t *inp,
				int *mx_tile, int *my_tile);
int tilemap_is_mouse_in_range(const glwin_input_t *inp);
void tilemap_tile_render(const glwin_input_t *inp, const int x,
			 const int y, int is_selected);

/* update */
void tilemap_pick_tile(const int x, const int y);
void tilemap_place_tile(const int x, const int y);
void tilemap_place_rect(const int x, const int y);
void tilemap_save(const char *outpath);
void tilemap_update_panning(const glwin_input_t *inp, const float dt);

/* Mappy (map editor) */
void tilemap_load_mappy(const char *path);
void tilemap_render_mappy(const glwin_input_t *inp, const int mx_tile,
			  const int my_tile);

#endif /* _MAPPY_TILEMAP_H_ */
