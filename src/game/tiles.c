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

static void tile_render(const int x, const int y, const uint8_t r,
			const uint8_t g, const uint8_t b)
{
	int xt = x * TILE_SIZE;
	int yt = y * TILE_SIZE;

	rdpq_fill_rect_border(xt, yt, xt + TILE_SIZE,
			      yt + TILE_SIZE, r, g, b, 2);
}

void tiles_render(void)
{
	for (int y = 0; y < TILES_Y; y++)
	{
		for (int x = 0; x < TILES_X; x++)
		{
			switch (tiles[y * TILES_X + x])
			{
			case TILE_TYPE_FLOOR:
				tile_render(x, y, 0xA, 0x0, 0x4);
				break;

			case TILE_TYPE_WALL:
				tile_render(x, y, 0x01, 0x15, 0x07);
				break;

			default:
				break;
			}
		}
	}
}
