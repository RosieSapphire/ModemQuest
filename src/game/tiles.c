#include "engine/util.h"

#include "game/tiles.h"
#include "game/player.h"

uint8_t tiles[TILES_DIM];

void tiles_init(void)
{
	const uint8_t tiles_new[TILES_DIM] = {
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 1, 1, 2, 1, 1, 2, 1, 1, 2,
		2, 1, 1, 1, 1, 1, 1, 1, 1, 2,
		2, 2, 1, 1, 1, 1, 1, 1, 2, 2,
		2, 1, 1, 1, 2, 2, 1, 1, 1, 2,
		2, 1, 1, 1, 2, 2, 1, 1, 1, 2,
		2, 2, 1, 1, 1, 1, 1, 1, 2, 2,
		2, 1, 1, 1, 1, 1, 1, 1, 1, 2,
		2, 1, 1, 2, 1, 1, 2, 1, 1, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	};

	memcpy(tiles, tiles_new, TILES_DIM * sizeof(*tiles));
}

void tiles_render(void)
{
	int x_off = 0;
	int y_off = 0;

	float plx, ply;

	player_get_pos_lerped(&plx, &ply);
	x_off = MIN(plx - ((DISPLAY_WIDTH >> 1) - (TILE_SIZE >> 1)), 0);
	y_off = MIN(ply - ((DISPLAY_HEIGHT >> 1) - (TILE_SIZE >> 1)), 0);

	for (int y = 0; y < TILES_Y; y++)
	{
		for (int x = 0; x < TILES_X; x++)
		{
			int xo = (x * TILE_SIZE) - x_off;
			int yo = (y * TILE_SIZE) - y_off;

			switch (tiles[y * TILES_X + x])
			{
			case TILE_TYPE_FLOOR:
				rdpq_fill_rect_border(xo, yo, xo + TILE_SIZE,
						      yo + TILE_SIZE,
						      0xA, 0x0, 0x4, 2);
				break;

			case TILE_TYPE_WALL:
				rdpq_fill_rect_border(xo, yo, xo + TILE_SIZE,
						      yo + TILE_SIZE,
						      0x1, 0x15, 0x7, 2);
				break;

			default:
				break;
			}
		}
	}
}
