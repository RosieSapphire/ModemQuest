#include <stdio.h>

#include "engine/tilemap.h"

#include "door.h"

door_t *door_get_from_pos(const int x, const int y)
{
	if (tilemap.tiles[y][x].type != TILE_TYPE_DOOR) {
		return NULL;
	}

	for (int i = 0; i < tilemap.num_doors; i++) {
		door_t *d = tilemap.doors + i;

		if (d->pos[0] == x && d->pos[1] == y) {
			return d;
		}
	}

	return NULL;
}

void door_duplicate(door_t *dst, const door_t *src)
{
	dst->map_index = src->map_index;
	dst->pos[0] = src->pos[0];
	dst->pos[1] = src->pos[1];
}

void door_destroy(door_t *d)
{
	d->map_index = 0xFF;
	d->pos[0] = d->pos[1] = 0;
}
