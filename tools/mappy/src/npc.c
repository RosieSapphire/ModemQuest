#include <stdio.h>

#define joypad_buttons_t void *
#include "game/tilemap.h"
#undef joypad_buttons_t
#include "npc.h"

npc_t *npc_get_from_pos(const int x, const int y)
{
	if (tilemap[y][x].type != TILE_TYPE_NPC)
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
