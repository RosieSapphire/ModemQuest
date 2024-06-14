#include <stdio.h>

#include "game/tilemap.h"
#include "nuklear_inst.h"
#include "endian.h"
#include "tilemap.h"
#include "npc.h"

#define TILEMAP_PANNING_SPEED 512

void tilemap_place_tile(const int x, const int y)
{
	tile_t *t = tilemap[y] + x;
	int type_old = t->type;

	t->type = tile_selected.type;
	t->col = tile_selected.col;
	if (t->type == TILE_TYPE_NPC)
	{
		npc_selected.pos[0] = x;
		npc_selected.pos[1] = y;
		tilemap_npc_cnt += (type_old != TILE_TYPE_NPC);
		tilemap_npcs[tilemap_npc_cnt - 1] = npc_selected;
	}
	else if (type_old == TILE_TYPE_NPC)
		--tilemap_npc_cnt;
}

void tilemap_pick_tile(const int x, const int y)
{
	const tile_t *t = tilemap[y] + x;

	tile_selected.type = t->type;
	tile_selected_colf.r =
		(float)((t->col & 0xF800) >> 11) / 31.0f;
	tile_selected_colf.g =
		(float)((t->col & 0x07C0) >> 6) / 31.0f;
	tile_selected_colf.b =
		(float)((t->col & 0x003E) >> 1) / 31.0f;

	if (t->type == TILE_TYPE_NPC)
		npc_selected = *npc_get_from_pos(x, y);
}

void tilemap_place_rect(const int mx_tile, const int my_tile)
{
	static int which_point, point_ax, point_ay, point_bx, point_by;

	switch (which_point)
	{
	case 0:
		point_ax = mx_tile;
		point_ay = my_tile;
		break;

	case 1:
		point_bx = mx_tile;
		point_by = my_tile;
		if (point_ax > point_bx)
		{
			int tmp = point_bx;

			point_bx = point_ax;
			point_ax = tmp;
		}
		if (point_ay > point_by)
		{
			int tmp = point_by;

			point_by = point_ay;
			point_ay = tmp;
		}
		for (int y = point_ay; y <= point_by; y++)
		{
			for (int x = point_ax; x <= point_bx; x++)
			{
				tilemap_place_tile(x, y);
			}
		}
		break;
	}

	which_point ^= 1;
}

void tilemap_save(const char *outpath)
{
	FILE *file = fopen(outpath, "wb");

	fwrite_ef16(&tilemap_w, file);
	fwrite_ef16(&tilemap_h, file);
	fwrite_ef16(&tilemap_npc_cnt, file);
	for (int y = 0; y < tilemap_h; y++)
	{
		for (int x = 0; x < tilemap_w; x++)
		{
			const tile_t *t = tilemap[y] + x;

			fwrite(&t->type, 1, 1, file);
			fwrite_ef16(&t->col, file);
		}
	}
	for (int i = 0; i < tilemap_npc_cnt; i++)
	{
		const npc_t *n = tilemap_npcs + i;

		fwrite(n->name, 1, NPC_NAME_MAX, file);
		fwrite_ef16(n->pos + 0, file);
		fwrite_ef16(n->pos + 1, file);
		fwrite_ef16(&n->dialogue_line_cnt, file);
		for (int j = 0; j < n->dialogue_line_cnt; j++)
		{
			const dialogue_line_t *dl = n->dialogue + j;

			fwrite(dl->speaker, 1, NPC_NAME_MAX, file);
			fwrite(dl->line, 1, DIALOGUE_LINE_MAX, file);
		}
	}

	fclose(file);
	printf("SAVED FILE '%s'\n", outpath);
}

void tilemap_update_panning(const glwin_input_t *inp, const float dt)
{
	if (npc_name_buf_state == NPC_NAME_BUF_STATE_ACTIVE)
		return;

	for (int i = 0; i < inp->shift_now + 1; i++)
	{
		const int px_min = -((glwin_w - 180) - (tilemap_w * TILE_SIZE));
		const int px_max = 0;
		const int py_min = -((glwin_h - 180) -
				(((tilemap_h >> 1) - 1) * TILE_SIZE));
		const int py_max = -128;

		tilemap_pan_x += (inp->d - inp->a) * TILEMAP_PANNING_SPEED * dt;
		if (tilemap_pan_x > px_max)
			tilemap_pan_x = px_max;
		if (tilemap_pan_x < px_min)
			tilemap_pan_x = px_min;
		tilemap_pan_y += (inp->s - inp->w) * TILEMAP_PANNING_SPEED * dt;
		if (tilemap_pan_y > py_max)
			tilemap_pan_y = py_max;
		if (tilemap_pan_y < py_min)
			tilemap_pan_y = py_min;
	}
}
