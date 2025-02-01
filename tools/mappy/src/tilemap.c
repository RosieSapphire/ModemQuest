#include <stdarg.h>
#include <string.h>
#include <GL/glew.h>

#include "config.h"
#include "render.h"
#include "endian.h"
#include "nuklear_inst.h"
#include "window.h"
#include "tilemap.h"
#include "npc.h"

#include "engine/tilemap.h"

#define TILEMAP_WIDTH_DEFAULT 10
#define TILEMAP_HEIGHT_DEFAULT 10
#define TILEMAP_PANNING_SPEED 512

#undef TILE_SIZE
#define TILE_SIZE (tilemap_is_zoomed ? 64 : 32)

uint16_t tilemap_width, tilemap_height, tilemap_num_npcs;
tile_t tilemap_tiles[TILEMAP_HEIGHT_MAX][TILEMAP_WIDTH_MAX];
npc_t tilemap_npcs[TILEMAP_NUM_NPCS_MAX];
tile_t tile_selected = { TILE_TYPE_FLOOR, 0xFFFF };
int tilemap_pan_x, tilemap_pan_y, tilemap_is_zoomed;

void tilemap_load_mappy(const char *path)
{
	tilemap_pan_x = tilemap_is_zoomed = 0;
	tilemap_pan_y = -128;

	FILE *file = fopen(path, "rb");

	/* If no file is found, we just initalize with the default */
	if (!file) {
		tilemap_width = TILEMAP_WIDTH_DEFAULT;
		tilemap_height = TILEMAP_HEIGHT_DEFAULT;
		tilemap_num_npcs = 0;
		for (int y = 0; y < TILEMAP_HEIGHT_MAX; y++) {
			for (int x = 0; x < TILEMAP_WIDTH_MAX; x++) {
				tilemap_tiles[y][x].type = TILE_TYPE_FLOOR;
				tilemap_tiles[y][x].col = 0xFFFF;
			}
		}

		return;
	}

	fread_ef16(&tilemap_width, file);
	fread_ef16(&tilemap_height, file);
	fread_ef16(&tilemap_num_npcs, file);
	for (int y = 0; y < tilemap_height; y++) {
		for (int x = 0; x < tilemap_width; x++) {
			tile_t *t = tilemap_tiles[y] + x;

			fread(&t->type, 1, 1, file);
			fread_ef16(&t->col, file);
		}
	}
	for (int i = 0; i < tilemap_num_npcs; i++) {
		npc_t *n = tilemap_npcs + i;

		fread(n->name, 1, NPC_NAME_MAX_LEN, file);
		fread_ef16(n->pos + 0, file);
		fread_ef16(n->pos + 1, file);
		fread_ef16(&n->num_dialogue_lines, file);
		for (int j = 0; j < n->num_dialogue_lines; j++) {
			dialogue_line_t *dl = n->dialogue + j;

			fread(dl->speaker, 1, NPC_NAME_MAX_LEN, file);
			fread(dl->line, 1, NPC_DIALOGUE_LINE_MAX_LEN, file);
		}
	}
	fclose(file);
}

void tilemap_unload_mappy(void)
{
	/* FIXME: More stuff here */
	tilemap_width = tilemap_height = tilemap_num_npcs = 0;
	memset(tilemap_npcs, 0, TILEMAP_NUM_NPCS_MAX * sizeof *tilemap_npcs);
	memset(tilemap_tiles, 0,
	       TILEMAP_WIDTH_MAX * TILEMAP_HEIGHT_MAX * sizeof **tilemap_tiles);
	tilemap_pan_x = tilemap_pan_y = tilemap_is_zoomed = 0;
}

void tilemap_update_mappy(const int mouse_tile[2], const float dt)
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
			const tile_t *tile = tilemap_tiles[y] + x;

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
		const tile_t *tile = tilemap_tiles[y] + x;

		r = ((tile->col & 0xF800) >> 11) / 31.0f;
		g = ((tile->col & 0x07C0) >> 6) / 31.0f;
		b = ((tile->col & 0x003E) >> 1) / 31.0f;
		a = ((tile->col & 0x0001) >> 0);
	}

	render_rect(x0, y0, x1, y1, r / 2, g / 2, b / 2, a);
	render_rect(x0 + pad, y0 + pad, x1 - pad, y1 - pad, r, g, b, a);
}

static void _tilemap_remove_npc(const uint16_t index)
{
	npc_t *npc = tilemap_npcs + index;

	/* if this is the last npcs in the list */
	npc_destroy(npc);
	if (index + 1 >= tilemap_num_npcs) {
		tilemap_num_npcs--;
		return;
	}

	for (uint16_t i = index + 1; i < tilemap_num_npcs; i++) {
		npc_t *to = tilemap_npcs + i - 1;
		npc_t *from = tilemap_npcs + i;

		npc_duplicate(to, from);
	}
	npc_destroy(tilemap_npcs + --tilemap_num_npcs);
}

void tilemap_place_tile(const int x, const int y)
{
	tile_t *t = tilemap_tiles[y] + x;
	int type_old = t->type;

	t->type = tile_selected.type;
	t->col = tile_selected.col;
	if (t->type == TILE_TYPE_NPC) {
		npc_selected.pos[0] = x;
		npc_selected.pos[1] = y;
		tilemap_num_npcs += (type_old != TILE_TYPE_NPC);
		tilemap_npcs[tilemap_num_npcs - 1] = npc_selected;
	} else if (type_old == TILE_TYPE_NPC) {
		int remove_ind = -1;
		for (uint16_t i = 0; i < tilemap_num_npcs; i++) {
			npc_t *n = tilemap_npcs + i;

			if (n->pos[0] == x && n->pos[1] == y) {
				remove_ind = i;
				break;
			}
		}
		if (remove_ind != -1) {
			printf("Tried to remove NPC %d\n", remove_ind);
			_tilemap_remove_npc(remove_ind);
		}
	}
}

void tilemap_pick_tile(const int x, const int y)
{
	const tile_t *t = tilemap_tiles[y] + x;

	tile_selected.type = t->type;
	tile_selected_colf.r = (float)((t->col & 0xF800) >> 11) / 31.0f;
	tile_selected_colf.g = (float)((t->col & 0x07C0) >> 6) / 31.0f;
	tile_selected_colf.b = (float)((t->col & 0x003E) >> 1) / 31.0f;

	if (t->type == TILE_TYPE_NPC)
		npc_selected = *npc_get_from_pos(x, y);
}

void tilemap_place_rect(const int mx_tile, const int my_tile)
{
	static int which_point, point_ax, point_ay, point_bx, point_by;

	switch (which_point) {
	case 0:
		point_ax = mx_tile;
		point_ay = my_tile;
		break;

	case 1:
		point_bx = mx_tile;
		point_by = my_tile;
		if (point_ax > point_bx) {
			int tmp = point_bx;

			point_bx = point_ax;
			point_ax = tmp;
		}
		if (point_ay > point_by) {
			int tmp = point_by;

			point_by = point_ay;
			point_ay = tmp;
		}
		for (int y = point_ay; y <= point_by; y++) {
			for (int x = point_ax; x <= point_bx; x++) {
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

	fwrite_ef16(&tilemap_width, file);
	fwrite_ef16(&tilemap_height, file);
	fwrite_ef16(&tilemap_num_npcs, file);
	for (int y = 0; y < tilemap_height; y++) {
		for (int x = 0; x < tilemap_width; x++) {
			const tile_t *t = tilemap_tiles[y] + x;

			fwrite(&t->type, 1, 1, file);
			fwrite_ef16(&t->col, file);
		}
	}
	for (int i = 0; i < tilemap_num_npcs; i++) {
		const npc_t *n = tilemap_npcs + i;

		fwrite(n->name, 1, NPC_NAME_MAX_LEN, file);
		fwrite_ef16(n->pos + 0, file);
		fwrite_ef16(n->pos + 1, file);
		fwrite_ef16(&n->num_dialogue_lines, file);
		for (int j = 0; j < n->num_dialogue_lines; j++) {
			const dialogue_line_t *dl = n->dialogue + j;

			fwrite(dl->speaker, 1, NPC_NAME_MAX_LEN, file);
			fwrite(dl->line, 1, NPC_DIALOGUE_LINE_MAX_LEN, file);
		}
	}

	fclose(file);
	printf("SAVED FILE '%s'\n", outpath);
}

void tilemap_update_panning(const float dt)
{
	if (npc_name_buf_state == NPC_NAME_BUF_STATE_ACTIVE)
		return;

	for (int i = 0; i < INPUT_GET_KEY(SHIFT, HELD) + 1; i++) {
		const int px_min = -((window_width - 180) -
				     (tilemap_width * TILE_SIZE_PXLS));
		const int px_max = 0;
		const int py_min =
			-((window_height - 180) -
			  (((tilemap_height >> 1) - 1) * TILE_SIZE_PXLS));
		const int py_max = -128;

		tilemap_pan_x +=
			(INPUT_GET_KEY(D, HELD) - INPUT_GET_KEY(A, HELD)) *
			TILEMAP_PANNING_SPEED * dt;
		if (tilemap_pan_x > px_max)
			tilemap_pan_x = px_max;
		if (tilemap_pan_x < px_min)
			tilemap_pan_x = px_min;
		tilemap_pan_y +=
			(INPUT_GET_KEY(S, HELD) - INPUT_GET_KEY(W, HELD)) *
			TILEMAP_PANNING_SPEED * dt;
		if (tilemap_pan_y > py_max)
			tilemap_pan_y = py_max;
		if (tilemap_pan_y < py_min)
			tilemap_pan_y = py_min;
	}
}
