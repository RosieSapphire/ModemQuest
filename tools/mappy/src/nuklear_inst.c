#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <GL/glew.h>

#include "config.h"
#include "glwin.h"
#include "tilemap.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear_inst.h"
#include "nuklear_glfw_gl3.h"

struct nk_context *nkctx;
struct nk_colorf tile_selected_colf;

void nuklear_inst_init(void)
{
	struct nk_font_atlas *atlas;

	nkctx = nk_glfw3_init(glwin, NK_GLFW3_INSTALL_CALLBACKS);
	nk_glfw3_font_stash_begin(&atlas);
	nk_glfw3_font_stash_end();
}

static void nuklear_inst_panel_project(const char *outpath,
				       const int num_spawns)
{
	int tilemap_w_prop = tilemap_w;
	int tilemap_h_prop = tilemap_h;

	nk_layout_row_static(nkctx, 16, 128, 1);
	nk_property_int(nkctx, "WIDTH:", 1, &tilemap_w_prop,
			TILES_W_MAX, 1, 1);
	nk_property_int(nkctx, "HEIGHT:", 1, &tilemap_h_prop,
			TILES_H_MAX, 1, 1);
	if (nk_button_label(nkctx, "RESET VIEW"))
	{
		tilemap_pan_x = 0;
		tilemap_pan_y = -128;
	}

	tilemap_w = (u16)tilemap_w_prop;
	tilemap_h = (u16)tilemap_h_prop;
	tilemap_resize(tilemap_w, tilemap_h);
}

static void nuklear_inst_panel_tile_selected(const glwin_input_t *inp,
					     const int mx_tile,
					     const int my_tile)
{
	char hover_pos_str[64], hover_type_str[128],
	     hover_col_str[128], tile_str[64];
	const tile_t *hovertile = tilemap[my_tile] + mx_tile;
	const char *tile_types[TILE_TYPE_CNT] = {
		"PLAYER_SPAWN", "FLOOR", "WALL"
	};
	const char *hover_types[TILE_TYPE_CNT] = {
		"  TILE: PLAYER_SPAWN,", "  TILE: FLOOR,", "  TILE: WALL,",
	};
	int tile_selected_type_prop = tile_selected.type;

	snprintf(tile_str, 64, tile_types[tile_selected.type]);
	nk_layout_row_static(nkctx, 16, 32, 2);
	nk_label(nkctx, "TILE: ", NK_LEFT);
	nk_layout_row_static(nkctx, 16, 128, 1);
	nk_property_int(nkctx, tile_str, 0, &tile_selected_type_prop,
			TILE_TYPE_CNT, 1, 1);
	nk_layout_row_static(nkctx, 128, 128, 1);
	nk_color_pick(nkctx, &tile_selected_colf, NK_RGB);
	tile_selected = (tile_t) {
		.type = tile_selected_type_prop,
		.col =  ((int)(tile_selected_colf.r * 31) << 11) |
			((int)(tile_selected_colf.g * 31) << 6) |
			((int)(tile_selected_colf.b * 31) << 1) | 1,
	};

	nk_layout_row_static(nkctx, 16, 128, 1);
	snprintf(hover_pos_str, 64, "  (%d, %d),", mx_tile, my_tile);
	snprintf(hover_type_str, 128, hover_types[hovertile->type]);
	snprintf(hover_col_str, 128, "  0x%.4X,", hovertile->col);
	if (!tilemap_is_mouse_inside(inp, mx_tile, my_tile))
		return;

	const char *labels[5] = {
		"HOVER: [", hover_pos_str, hover_type_str, hover_col_str, "]",
	};

	for (int i = 0; i < 5; i++)
		nk_label(nkctx, labels[i], NK_LEFT);
}

void nuklear_inst_render(const glwin_input_t *inp, const char *outpath,
			 const int mx_tile, const int my_tile)
{
	int num_spawns = 0;
	char project_str[64];

	for (int y = 0; y < tilemap_h; y++)
		for (int x = 0; x < tilemap_w; x++)
			num_spawns += (tilemap[y][x].type ==
				 TILE_TYPE_PLAYER_SPAWN);

	nk_glfw3_new_frame();
	snprintf(project_str, 64, "PROJECT '%s' (%d spawns)",
		 outpath, num_spawns);

	if (nk_begin(nkctx, project_str,
		     nk_rect(0, 0, WIN_WID - 180, 128),
		     NK_WINDOW_BORDER | NK_WINDOW_TITLE))
		nuklear_inst_panel_project(outpath, num_spawns);
	nk_end(nkctx);

	if (nk_begin(nkctx, "TILE SELECTED",
		     nk_rect(WIN_WID - 180, 0, 180, WIN_HEI),
		     NK_WINDOW_BORDER | NK_WINDOW_TITLE))
		nuklear_inst_panel_tile_selected(inp, mx_tile, my_tile);
	nk_end(nkctx);

	nk_glfw3_render(NK_ANTI_ALIASING_ON, NK_VERT_BUF_MAX,
			NK_ELEM_BUF_MAX);
}

void nuklear_inst_terminate(void)
{
	nk_glfw3_shutdown();
}
