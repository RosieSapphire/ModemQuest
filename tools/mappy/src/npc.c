#include <stdio.h>

#include "npc.h"

#include "engine/tilemap.h"

npc_t *npc_get_from_pos(const int x, const int y)
{
	if (tilemap_tiles[y][x].type != TILE_TYPE_NPC)
		return (NULL);

	for (int i = 0; i < tilemap_num_npcs; i++) {
		npc_t *n = tilemap_npcs + i;

		printf("(%d, %d): %d/%d '%s'\n", n->pos[0], n->pos[1], i + 1,
		       tilemap_num_npcs, n->name);
		if (n->pos[0] == x && n->pos[1] == y) {
			printf("DECIDED ON %d ('%s')\n", i, n->name);
			return (n);
		}
	}

	return (NULL);
}
