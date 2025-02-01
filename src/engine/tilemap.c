#include <libdragon.h>

#include "config.h"
#include "rdpq_ext.h"

#include "engine/player.h"
#include "engine/tilemap.h"

uint16_t tilemap_width = 0, tilemap_height = 0, tilemap_num_npcs = 0;
tile_t tilemap_tiles[TILEMAP_HEIGHT_MAX][TILEMAP_WIDTH_MAX];
npc_t tilemap_npcs[TILEMAP_NUM_NPCS_MAX];

void tilemap_init(const char *path, vec2i spawn_pos)
{
	FILE *file = fopen(path, "rb");

	assertf(file, "Failed to load tilemap from '%s'\n", path);

	fread(&tilemap_width, 2, 1, file);
	fread(&tilemap_height, 2, 1, file);
	fread(&tilemap_num_npcs, 2, 1, file);

	/* tiles */
	for (int y = 0; y < tilemap_height; y++) {
		for (int x = 0; x < tilemap_width; x++) {
			tile_t *t = tilemap_tiles[y] + x;

			fread(&t->type, 1, 1, file);
			fread(&t->col, 2, 1, file);
		}
	}

	/* npcs */
	for (int i = 0; i < tilemap_num_npcs; i++) {
		npc_t *n = tilemap_npcs + i;

		fread(n->name, 1, NPC_NAME_MAX_LEN, file);
		fread(n->pos, 2, 2, file);
		fread(&n->num_dialogue_lines, 2, 1, file);

		/* npc dialogue */
		for (int j = 0; j < n->num_dialogue_lines; j++) {
			dialogue_line_t *dl = n->dialogue + j;

			fread(dl->speaker, 1, NPC_NAME_MAX_LEN, file);
			fread(dl->line, 1, NPC_DIALOGUE_LINE_MAX_LEN, file);
		}

		n->state = NPC_STATE_IDLE;
		n->dialogue_cur = -1;
		n->dialogue_char_cur = 0;
	}

	fclose(file);

	/* getting player spawn position (will only pick first one) */
	int has_found_spawn = false;
	for (int y = 0; y < tilemap_height; y++) {
		for (int x = 0; x < tilemap_width; x++) {
			if (tilemap_tiles[y][x].type !=
			    TILE_TYPE_PLAYER_SPAWN) {
				continue;
			}

			vec2i_set(spawn_pos, x, y);
			has_found_spawn = true;
			break;
		}
	}

	assertf(has_found_spawn, "Unable to find spawn point in tilemap '%s'\n",
		path);

	debugf("LOADED TILEMAP '%s': [%d, %d], %d npc(s)\n", path,
	       tilemap_width, tilemap_height, tilemap_num_npcs);
}

void tilemap_update(void)
{
	for (int i = 0; i < tilemap_num_npcs; i++) {
		npc_player_interact(tilemap_npcs + i);
	}
}

void tilemap_render(const float subtick)
{
	vec2f player_pos;
	player_get_pos_lerped(player_pos, subtick);

	vec2i offset = { 0, 0 };

	if (player_pos[0] - PLAYER_RENDER_POS_X_MAX < 0) {
		offset[0] = 0;
	} else {
		offset[0] = player_pos[0] - PLAYER_RENDER_POS_X_MAX;
	}

	if (player_pos[1] - PLAYER_RENDER_POS_Y_MAX < 0) {
		offset[1] = 0;
	} else {
		offset[1] = player_pos[1] - PLAYER_RENDER_POS_Y_MAX;
	}

	/* TODO: Make the bottom right corner of the map clamp properly */
	/*
	if (offset[0] > 10 * TILE_SIZE_PXLS) {
		offset[0] = 10 * TILE_SIZE_PXLS;
	}
	*/

	const int x_min = offset[0] >> 5;
	const int x_max = x_min + 11;
	const int y_min = offset[1] >> 5;
	const int y_max = y_min + 11;
	for (int y = y_min; y < y_max; y++) {
		for (int x = x_min; x < x_max; x++) {
			int xo = (x * TILE_SIZE_PXLS) - offset[0];
			int yo = (y * TILE_SIZE_PXLS) - offset[1];

			rdpq_fill_rect_border(xo, yo, xo + TILE_SIZE_PXLS,
					      yo + TILE_SIZE_PXLS,
					      tilemap_tiles[y][x].col, 2);
		}
	}
}

void tilemap_render_npc_dialogue_boxes(void)
{
	for (int i = 0; i < tilemap_num_npcs; i++) {
		npc_dialogue_box_render(tilemap_npcs + i);
	}
}

void tilemap_terminate(void)
{
	/*
	memset(npcs, 0, sizeof *npcs * TILEMAP_NUM_NPCS_MAX);
	memset(tilemap, 0, sizeof **tilemap * TILEMAP_WIDTH_MAX * TILEMAP_HEIGHT_MAX);
	*/
	tilemap_width = tilemap_height = tilemap_num_npcs = 0;
	debugf("UNLOADED TILEMAP\n");
}
