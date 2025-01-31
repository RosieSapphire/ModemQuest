#include <libdragon.h>

#include "config.h"
#include "rdpq_ext.h"

#include "engine/npc.h"
#include "engine/player.h"
#include "engine/tilemap.h"

static uint16_t width, height, num_npcs;
tile_t map[TILEMAP_HEIGHT_MAX][TILEMAP_WIDTH_MAX];
npc_t npcs[TILEMAP_NUM_NPCS_MAX];

void tilemap_init(const char *path, vec2i spawn_pos)
{
	FILE *file = fopen(path, "rb");

	assertf(file, "Failed to load tilemap from '%s'\n", path);

	fread(&width, 2, 1, file);
	fread(&height, 2, 1, file);
	fread(&num_npcs, 2, 1, file);

	/* tiles */
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			tile_t *t = map[y] + x;

			fread(&t->type, 1, 1, file);
			fread(&t->col, 2, 1, file);
		}
	}

	/* npcs */
	for (int i = 0; i < num_npcs; i++) {
		npc_t *n = npcs + i;

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
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (map[y][x].type != TILE_TYPE_PLAYER_SPAWN) {
				continue;
			}

			vec2i_set(spawn_pos, x, y);
			has_found_spawn = true;
			break;
		}
	}

	assertf(has_found_spawn, "Unable to find spawn point in map '%s'\n",
		path);

	debugf("LOADED TILEMAP '%s': [%d, %d], %d npc(s)\n", path, width,
	       height, num_npcs);
}

void tilemap_update(void)
{
	for (int i = 0; i < num_npcs; i++) {
		npc_player_interact(npcs + i);
	}
}

void tilemap_render(const float subtick)
{
	vec2f player_pos;
	player_get_pos_lerped(player_pos, subtick);

	vec2i offset = {
		fmaxf(player_pos[0] -
			      ((DISPLAY_WIDTH >> 1) - (TILE_SIZE_PXLS >> 1)),
		      0),
		fmaxf(player_pos[1] -
			      ((DISPLAY_HEIGHT >> 1) - (TILE_SIZE_PXLS >> 1)),
		      0),
	};

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int xo = (x * TILE_SIZE_PXLS) - offset[0];
			int yo = (y * TILE_SIZE_PXLS) - offset[1];

			rdpq_fill_rect_border(xo, yo, xo + TILE_SIZE_PXLS,
					      yo + TILE_SIZE_PXLS,
					      map[y][x].col, 2);
		}
	}
}

void tilemap_render_npc_dialogue_boxes(void)
{
	for (int i = 0; i < num_npcs; i++) {
		npc_dialogue_box_render(npcs + i);
	}
}

tile_t *tilemap_get_tile(const int x, const int y)
{
	return map[y] + x;
}

void tilemap_terminate(void)
{
	/*
	memset(npcs, 0, sizeof *npcs * TILEMAP_NUM_NPCS_MAX);
	memset(map, 0, sizeof **map * TILEMAP_WIDTH_MAX * TILEMAP_HEIGHT_MAX);
	*/
	width = height = num_npcs = 0;
	debugf("UNLOADED TILEMAP\n");
}
