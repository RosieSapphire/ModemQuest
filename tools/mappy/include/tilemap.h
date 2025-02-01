#ifndef _MAPPY_TILEMAP_H_
#define _MAPPY_TILEMAP_H_

#include "engine/tilemap.h"

extern tile_t tile_selected;
extern int tilemap_pan_x, tilemap_pan_y, tilemap_is_zoomed;

/* Mappy (map editor) */
void tilemap_load_mappy(const char *path);
void tilemap_update_mappy(const int mouse_tile[2], const float dt);
void tilemap_render_mappy(const int mx_tile, const int my_tile);
void tilemap_unload_mappy(void);

/* base */
int tilemap_is_mouse_in_range(void);
void tilemap_tile_render(const int x, const int y, int is_selected);

/* update */
void tilemap_pick_tile(const int x, const int y);
void tilemap_place_tile(const int x, const int y);
void tilemap_place_rect(const int x, const int y);
void tilemap_save(const char *outpath);
void tilemap_update_panning(const float dt);

#endif /* _MAPPY_TILEMAP_H_ */
