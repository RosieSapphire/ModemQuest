#include "engine/util.h"

#include "game/tiles.h"
#include "game/player.h"

uint16_t tiles_w, tiles_h;
uint8_t tiles[TILES_H_MAX][TILES_W_MAX];

void tiles_init(const char *path, int *player_spawn_x, int *player_spawn_y)
{
	FILE *file = fopen(path, "rb");

	assertf(file, "File loading fucked up '%s'\n", path);
	fread(&tiles_w, 2, 1, file);
	fread(&tiles_h, 2, 1, file);
	for (int y = 0; y < tiles_h; y++)
		for (int x = 0; x < tiles_w; x++)
			fread(tiles[y] + x, 1, 1, file);
	fclose(file);

	debugf("LOADED TILEMAP (%d, %d)\n", tiles_w, tiles_h);
	for (int y = 0; y < tiles_h; y++)
	{
		for (int x = 0; x < tiles_w; x++)
		{
			if (tiles[y][x] == TILE_TYPE_PLAYER_SPAWN)
			{
				*player_spawn_x = x;
				*player_spawn_y = y;
			}
		}
	}
}

void tiles_render(void)
{
	int x_off = 0;
	int y_off = 0;

	float plx, ply;

	player_get_pos_lerped(&plx, &ply);
	x_off = MIN(plx - ((DISPLAY_WIDTH >> 1) - (TILE_SIZE >> 1)), 0);
	y_off = MIN(ply - ((DISPLAY_HEIGHT >> 1) - (TILE_SIZE >> 1)), 0);

	for (int y = 0; y < tiles_h; y++)
	{
		for (int x = 0; x < tiles_w; x++)
		{
			int xo = (x * TILE_SIZE) - x_off;
			int yo = (y * TILE_SIZE) - y_off;

			switch (tiles[y][x])
			{
			case TILE_TYPE_PLAYER_SPAWN:
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
