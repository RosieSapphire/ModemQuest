#include "engine/util.h"
#include "engine/config.h"

#include "game/tiles.h"
#include "game/player.h"

uint16_t tiles_w, tiles_h;
tile_t tiles[TILES_H_MAX][TILES_W_MAX];

void tiles_init(const char *path, int *player_spawn_x, int *player_spawn_y)
{
	FILE *file = fopen(path, "rb");

	assertf(file, "File loading fucked up '%s'\n", path);
	fread(&tiles_w, 2, 1, file);
	fread(&tiles_h, 2, 1, file);
	for (int y = 0; y < tiles_h; y++)
	{
		for (int x = 0; x < tiles_w; x++)
		{
			tile_t *t = tiles[y] + x;

			fread(&t->type, 1, 1, file);
			fread(&t->col, 2, 1, file);
		}
	}
	fclose(file);

	/* count player spawns */
	int player_spawn_cnt = 0;
	int *player_spawn_xs = malloc(0);
	int *player_spawn_ys = malloc(0);

	for (int y = 0; y < tiles_h; y++)
	{
		for (int x = 0; x < tiles_w; x++)
		{
			if (tiles[y][x].type != TILE_TYPE_PLAYER_SPAWN)
				continue;

			player_spawn_cnt++;
			player_spawn_xs = realloc(player_spawn_xs,
					sizeof(*player_spawn_xs) *
					player_spawn_cnt);
			player_spawn_ys = realloc(player_spawn_ys,
					sizeof(*player_spawn_ys) *
					player_spawn_cnt);
			player_spawn_xs[player_spawn_cnt - 1] = x;
			player_spawn_ys[player_spawn_cnt - 1] = y;
		}
	}

	assertf(player_spawn_cnt, "ERROR: Tilemap from '%s' doesn't "
		"have any player spawns\n", path);

	/* choose which one to spawn at */
	int which_spawn = rand() % player_spawn_cnt;

	*player_spawn_x = player_spawn_xs[which_spawn];
	*player_spawn_y = player_spawn_ys[which_spawn];
	free(player_spawn_xs);
	free(player_spawn_ys);

	debugf("LOADED TILEMAP '%s': [%d, %d] %d spawn(s)\n",
	       path, tiles_w, tiles_h, player_spawn_cnt);
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

			rdpq_fill_rect_border(xo, yo, xo + TILE_SIZE,
					      yo + TILE_SIZE,
					      tiles[y][x].col, 2);
		}
	}
}
