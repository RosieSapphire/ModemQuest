#include <stdio.h>
#include <string.h>

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

void npc_duplicate(npc_t *to, const npc_t *from)
{
	strncpy(to->name, from->name, NPC_NAME_MAX_LEN);
	to->pos[0] = from->pos[0];
	to->pos[1] = from->pos[1];
	to->num_dialogue_lines = from->num_dialogue_lines;

	for (uint16_t i = 0; i < to->num_dialogue_lines; i++) {
		dialogue_line_t *dto = to->dialogue + i;
		const dialogue_line_t *dfrom = from->dialogue + i;

		strncpy(dto->speaker, dfrom->speaker, NPC_NAME_MAX_LEN);
		strncpy(dto->line, dfrom->line, NPC_DIALOGUE_LINE_MAX_LEN);
	}
}

void npc_destroy(npc_t *n)
{
	memset(n->name, 0, NPC_NAME_MAX_LEN);
	memset(n->pos, 0, sizeof *n->pos * 2);

	for (uint16_t i = 0; i < n->num_dialogue_lines; i++) {
		dialogue_line_t *d = n->dialogue + i;

		memset(d->speaker, 0, NPC_NAME_MAX_LEN);
		memset(d->line, 0, NPC_DIALOGUE_LINE_MAX_LEN);
	}

	n->num_dialogue_lines = 0;
}
