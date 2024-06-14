#include <stdarg.h>
#include <GL/glew.h>

#include "config.h"
#include "game/tilemap.h"
#include "render.h"
#include "endian.h"
#include "nuklear_inst.h"
#include "tilemap.h"

#define TILEMAP_W_DEFAULT 10
#define TILEMAP_H_DEFAULT 10

#undef TILE_SIZE
#define TILE_SIZE (tilemap_is_zoomed ? 64 : 32)

u16 tilemap_w, tilemap_h, tilemap_npc_cnt;
tile_t tilemap[TILEMAP_H_MAX][TILEMAP_W_MAX];
npc_t tilemap_npcs[TILEMAP_NPC_MAX];
tile_t tile_selected = {TILE_TYPE_FLOOR, 0xFFFF};
int tilemap_pan_x, tilemap_pan_y, tilemap_is_zoomed;

static void tilemap_read_file(FILE *file)
{
	fread_ef16(&tilemap_w, file);
	fread_ef16(&tilemap_h, file);
	fread_ef16(&tilemap_npc_cnt, file);
	for (int y = 0; y < tilemap_h; y++)
	{
		for (int x = 0; x < tilemap_w; x++)
		{
			tile_t *t = tilemap[y] + x;

			fread(&t->type, 1, 1, file);
			fread_ef16(&t->col, file);
		}
	}
	for (int i = 0; i < tilemap_npc_cnt; i++)
	{
		npc_t *n = tilemap_npcs + i;

		fread(n->name, 1, NPC_NAME_MAX, file);
		fread_ef16(n->pos + 0, file);
		fread_ef16(n->pos + 1, file);
		fread_ef16(&n->dialogue_line_cnt, file);
		for (int j = 0; j < n->dialogue_line_cnt; j++)
		{
			dialogue_line_t *dl = n->dialogue + j;

			fread(dl->speaker, 1, NPC_NAME_MAX, file);
			fread(dl->line, 1, DIALOGUE_LINE_MAX, file);
		}
	}
}

void tilemap_load_mappy(const char *path)
{
	tilemap_pan_x = 0;
	tilemap_pan_y = -128;
	tilemap_is_zoomed = 0;

	FILE *file = fopen(path, "rb");

	/* If no file is found, we just initalize with the default */
	if (!file)
	{
		tilemap_w = TILEMAP_W_DEFAULT;
		tilemap_h = TILEMAP_H_DEFAULT;
		tilemap_npc_cnt = 0;
		for (int y = 0; y < TILEMAP_H_MAX; y++)
		{
			for (int x = 0; x < TILEMAP_W_MAX; x++)
			{
				tilemap[y][x].type = TILE_TYPE_FLOOR;
				tilemap[y][x].col = 0xFFFF;
			}
		}

		return;
	}

	tilemap_read_file(file);
	fclose(file);
}

void tilemap_update(const glwin_input_t *inp, const int mx_tile,
		    const int my_tile, const float dt)
{
	tilemap_update_panning(inp, dt);

	tilemap_is_zoomed ^= (inp->z_now && !inp->z_last);

	if (!tilemap_is_mouse_in_range(inp))
		return;

	static int mx_tile_placed_last = -1;
	static int my_tile_placed_last = -1;

	if (inp->lmb_now &&
	    (mx_tile_placed_last != mx_tile ||
	     my_tile_placed_last != my_tile))
	{
		tilemap_place_tile(mx_tile, my_tile);
		mx_tile_placed_last = mx_tile;
		my_tile_placed_last = my_tile;
	}

	if (inp->mmb_now && !inp->mmb_last)
		tilemap_pick_tile(mx_tile, my_tile);

	if (inp->rmb_now && !inp->rmb_last)
		tilemap_place_rect(mx_tile, my_tile);
}

void tilemap_tile_render(const glwin_input_t *inp, const int x,
			 const int y, int is_selected)
{
	if (x < 0 || x >= tilemap_w || y < 0 || y >= tilemap_h)
		return;

	const int pad = 3;
	const int x0 = (x * TILE_SIZE) - tilemap_pan_x;
	const int y0 = (y * TILE_SIZE) - tilemap_pan_y;
	const int x1 = x0 + TILE_SIZE;
	const int y1 = y0 + TILE_SIZE;
	float r, g, b, a;

	if (is_selected)
	{
		if (!tilemap_is_mouse_in_range(inp))
			goto render_tile_already_there_instead;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		r = tile_selected_colf.r;
		g = tile_selected_colf.g;
		b = tile_selected_colf.b;
		a = 0.5f;
		glDisable(GL_BLEND);
	}
	else
	{
render_tile_already_there_instead:
		const tile_t *tile = tilemap[y] + x;

		r = ((tile->col & 0xF800) >> 11) / 31.0f;
		g = ((tile->col & 0x07C0) >> 6) / 31.0f;
		b = ((tile->col & 0x003E) >> 1) / 31.0f;
		a = ((tile->col & 0x0001) >> 0);
	}

	render_rect(x0, y0, x1, y1, r / 2, g / 2, b / 2, a);
	render_rect(x0 + pad, y0 + pad, x1 - pad, y1 - pad, r, g, b, a);
}

int tilemap_is_mouse_in_range(const glwin_input_t *inp)
{
	int in_proj_panel = (inp->mx_now < glwin_w - 180 && inp->my_now < 128);
	int in_tile_panel = (inp->mx_now > glwin_w - 180);
	int in_npc_panel  = (inp->mx_now < glwin_w - 180 &&
			     inp->my_now > glwin_h - 180) &&
			    tile_selected.type == TILE_TYPE_NPC;

	return (!in_proj_panel && !in_tile_panel && !in_npc_panel);
}
