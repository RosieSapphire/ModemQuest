#include <stdarg.h>
#include <string.h>
#include <GL/glew.h>

#include "config.h"
#include "game/tilemap.h"
#include "render.h"
#include "endian.h"
#include "nuklear_inst.h"
#include "window.h"
#include "tilemap.h"

#define TILEMAP_WIDTH_DEFAULT 10
#define TILEMAP_HEIGHT_DEFAULT 10

#undef TILE_SIZE
#define TILE_SIZE (tilemap_is_zoomed ? 64 : 32)

u16 tilemap_width, tilemap_height, tilemap_num_npcs;
tile_t tilemap[TILEMAP_HEIGHT_MAX][TILEMAP_WIDTH_MAX];
npc_t tilemap_npcs[TILEMAP_NPC_MAX];
tile_t tile_selected = { TILE_TYPE_FLOOR, 0xFFFF };
int tilemap_pan_x, tilemap_pan_y, tilemap_is_zoomed;

void tilemap_load_mappy(const char *path)
{
	tilemap_pan_x = 0;
	tilemap_pan_y = -128;
	tilemap_is_zoomed = 0;

	FILE *file = fopen(path, "rb");

	/* If no file is found, we just initalize with the default */
	if (!file) {
		tilemap_width = TILEMAP_WIDTH_DEFAULT;
		tilemap_height = TILEMAP_HEIGHT_DEFAULT;
		tilemap_num_npcs = 0;
		for (int y = 0; y < TILEMAP_HEIGHT_MAX; y++) {
			for (int x = 0; x < TILEMAP_WIDTH_MAX; x++) {
				tilemap[y][x].type = TILE_TYPE_FLOOR;
				tilemap[y][x].col = 0xFFFF;
			}
		}

		return;
	}

	fread_ef16(&tilemap_width, file);
	fread_ef16(&tilemap_height, file);
	fread_ef16(&tilemap_num_npcs, file);
	for (int y = 0; y < tilemap_height; y++) {
		for (int x = 0; x < tilemap_width; x++) {
			tile_t *t = tilemap[y] + x;

			fread(&t->type, 1, 1, file);
			fread_ef16(&t->col, file);
		}
	}
	for (int i = 0; i < tilemap_num_npcs; i++) {
		npc_t *n = tilemap_npcs + i;

		fread(n->name, 1, NPC_NAME_MAX, file);
		fread_ef16(n->pos + 0, file);
		fread_ef16(n->pos + 1, file);
		fread_ef16(&n->dialogue_line_cnt, file);
		for (int j = 0; j < n->dialogue_line_cnt; j++) {
			dialogue_line_t *dl = n->dialogue + j;

			fread(dl->speaker, 1, NPC_NAME_MAX, file);
			fread(dl->line, 1, DIALOGUE_LINE_MAX, file);
		}
	}
	fclose(file);
}

void tilemap_unload_mappy(void)
{
	/* FIXME: More stuff here */
	tilemap_width = tilemap_height = tilemap_num_npcs = 0;
	memset(tilemap_npcs, 0, TILEMAP_NPC_MAX * sizeof *tilemap_npcs);
	memset(tilemap, 0,
	       TILEMAP_WIDTH_MAX * TILEMAP_HEIGHT_MAX * sizeof **tilemap);
	tilemap_pan_x = tilemap_pan_y = tilemap_is_zoomed = 0;
}

void tilemap_update(const int mouse_tile[2], const float dt)
{
	tilemap_update_panning(dt);

	tilemap_is_zoomed ^= INPUT_GET_KEY(Z, PRESSED);

	if (!tilemap_is_mouse_in_range())
		return;

	static int mouse_tile_placed_last[2] = { -1, -1 };

	if (INPUT_GET_MB(LEFT, HELD) &&
	    (mouse_tile_placed_last[0] != mouse_tile[0] ||
	     mouse_tile_placed_last[1] != mouse_tile[1])) {
		tilemap_place_tile(mouse_tile[0], mouse_tile[1]);
		mouse_tile_placed_last[0] = mouse_tile[0];
		mouse_tile_placed_last[1] = mouse_tile[1];
	}

	if (INPUT_GET_MB(MIDDLE, PRESSED))
		tilemap_pick_tile(mouse_tile[0], mouse_tile[1]);

	if (INPUT_GET_MB(RIGHT, PRESSED))
		tilemap_place_rect(mouse_tile[0], mouse_tile[1]);
}

void tilemap_tile_render(const int x, const int y, int is_selected)
{
	if (x < 0 || x >= tilemap_width || y < 0 || y >= tilemap_height)
		return;

	const int pad = 3;
	const int x0 = (x * TILE_SIZE) - tilemap_pan_x;
	const int y0 = (y * TILE_SIZE) - tilemap_pan_y;
	const int x1 = x0 + TILE_SIZE;
	const int y1 = y0 + TILE_SIZE;
	float r, g, b, a;

	if (is_selected) {
		if (!tilemap_is_mouse_in_range()) {
			const tile_t *tile = tilemap[y] + x;

			r = ((tile->col & 0xF800) >> 11) / 31.0f;
			g = ((tile->col & 0x07C0) >> 6) / 31.0f;
			b = ((tile->col & 0x003E) >> 1) / 31.0f;
			a = ((tile->col & 0x0001) >> 0);
			render_rect(x0, y0, x1, y1, r / 2, g / 2, b / 2, a);
			render_rect(x0 + pad, y0 + pad, x1 - pad, y1 - pad, r,
				    g, b, a);
			return;
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		r = tile_selected_colf.r;
		g = tile_selected_colf.g;
		b = tile_selected_colf.b;
		a = 0.5f;
		glDisable(GL_BLEND);
	} else {
		const tile_t *tile = tilemap[y] + x;

		r = ((tile->col & 0xF800) >> 11) / 31.0f;
		g = ((tile->col & 0x07C0) >> 6) / 31.0f;
		b = ((tile->col & 0x003E) >> 1) / 31.0f;
		a = ((tile->col & 0x0001) >> 0);
	}

	render_rect(x0, y0, x1, y1, r / 2, g / 2, b / 2, a);
	render_rect(x0 + pad, y0 + pad, x1 - pad, y1 - pad, r, g, b, a);
}

int tilemap_is_mouse_in_range(void)
{
	int in_proj_panel = (INPUT_GET_MOUSE(X, NOW) < window_width - 180 &&
			     INPUT_GET_MOUSE(Y, NOW) < 128);
	int in_tile_panel = (INPUT_GET_MOUSE(X, NOW) > window_width - 180);
	int in_npc_panel = (INPUT_GET_MOUSE(X, NOW) < window_width - 180 &&
			    INPUT_GET_MOUSE(Y, NOW) > window_height - 180) &&
			   tile_selected.type == TILE_TYPE_NPC;

	return (!in_proj_panel && !in_tile_panel && !in_npc_panel);
}
