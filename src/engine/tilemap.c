#include <libdragon.h>

#include "config.h"
#include "rdpq_ext.h"

#include "engine/player.h"
#include "engine/tilemap.h"

struct tilemap tilemap;

void tilemap_init(const char *path, vec2s spawn_pos)
{
	FILE *file = asset_fopen(path, NULL);

	assertf(file, "Failed to load tilemap from '%s'\n", path);

	fread(&tilemap.map_index, 2, 1, file);
	fread(&tilemap.width, 1, 1, file);
	fread(&tilemap.height, 1, 1, file);
	fread(&tilemap.npc_count, 1, 1, file);
	fread(&tilemap.door_count, 1, 1, file);

	/* tiles */
	for (int y = 0; y < tilemap.height; y++) {
		for (int x = 0; x < tilemap.width; x++) {
			struct tile *t = tilemap.tiles[y] + x;

			fread(&t->type, 1, 1, file);
			fread(&t->color, 2, 1, file);
		}
	}

	/* npcs */
	tilemap.npcs = calloc(tilemap.npc_count, sizeof(*tilemap.npcs));
	for (int i = 0; i < tilemap.npc_count; i++) {
		struct npc *n = tilemap.npcs + i;

		debugf("NPC:\n");
		fread(n->name, 1, NPC_NAME_MAX_LEN, file);
		debugf("\tName: '%s'\n", n->name);
		fread(n->pos + 0, 2, 1, file);
		fread(n->pos + 1, 2, 1, file);
		debugf("\tPos: %d, %d\n", n->pos[0], n->pos[1]);
		fread(&n->num_dialogue_lines, 2, 1, file);
		debugf("\tLine Count: %u\n", n->num_dialogue_lines);
		n->dialogue =
			calloc(n->num_dialogue_lines, sizeof(*n->dialogue));
		debugf("\tLines:\n");
		for (int j = 0; j < n->num_dialogue_lines; j++) {
			struct dialogue_line *line = n->dialogue + j;

			fread(&line->speaker, 1, 1, file);
			debugf("\t\tSpeaker: %u\n", line->speaker);
			fread(line->line, 1, NPC_DIALOGUE_LINE_MAX_LEN, file);
			debugf("\t\tLine: '%s'\n", line->line);
		}

		n->state = NPC_STATE_IDLE;
		n->dialogue_cur = -1;
		n->dialogue_char_cur = -1;
	}

	/* doors */
	for (int i = 0; i < tilemap.door_count; i++) {
		struct door *d = tilemap.doors + i;

		fread(&d->map_index, 1, 1, file);
	}

	fclose(file);

	/* getting player spawn position (will only pick first one) */
	int has_found_spawn = false;
	for (int y = 0; y < tilemap.height; y++) {
		for (int x = 0; x < tilemap.width; x++) {
			if (tilemap.tiles[y][x].type !=
			    TILE_TYPE_PLAYER_SPAWN) {
				continue;
			}

			vec2s_set(spawn_pos, x, y);
			has_found_spawn = true;
			break;
		}
	}

	assertf(has_found_spawn, "Unable to find spawn point in tilemap '%s'\n",
		path);

	debugf("LOADED TILEMAP '%s': [%d, %d], %d npc(s)\n", path,
	       tilemap.width, tilemap.height, tilemap.npc_count);
}

void tilemap_update(void)
{
	for (int i = 0; i < tilemap.npc_count; i++) {
		npc_player_interact(tilemap.npcs + i);
	}
}

void tilemap_render(const float subtick)
{
	vec2f player_pos;
	player_get_pos_lerped(player_pos, subtick);

	vec2s offset = { 0, 0 };

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
					      tilemap.tiles[y][x].color, 2);
		}
	}
}

void tilemap_render_npc_dialogue_boxes(void)
{
	for (int i = 0; i < tilemap.npc_count; i++) {
		npc_dialogue_box_render(tilemap.npcs + i, player.name);
	}
}

void tilemap_free(void)
{
	tilemap.width = tilemap.height = tilemap.npc_count =
		tilemap.door_count = tilemap.map_index = 0;
	debugf("UNLOADED TILEMAP\n");
}
