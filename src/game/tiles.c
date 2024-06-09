#include "util.h"
#include "config.h"

#include "game/tiles.h"
#include "game/player.h"

u16 tiles_w, tiles_h;
tile_t tiles[TILES_H_MAX][TILES_W_MAX];

static int tiles_init_spawn_pos(vec2i_t *spawnpos)
{
	/* count player spawns */
	int spawn_cnt = 0;
	vec2i_t *spawn_poss = malloc(0);

	for (int y = 0; y < tiles_h; y++)
	{
		for (int x = 0; x < tiles_w; x++)
		{
			if (tiles[y][x].type != TILE_TYPE_PLAYER_SPAWN)
				continue;

			spawn_cnt++;
			spawn_poss = realloc(spawn_poss, sizeof(*spawn_poss) *
					     spawn_cnt);
			spawn_poss[spawn_cnt - 1] = VEC2I(x, y);
		}
	}

	assertf(spawn_cnt, "ERROR: Tilemap doesn't have any player spawns\n");

	/* choose which one to spawn at */
	int which_spawn = rand() % spawn_cnt;

	*spawnpos = spawn_poss[which_spawn];
	free(spawn_poss);

	return (spawn_cnt);
}

void tiles_init(const char *path, vec2i_t *spawnpos)
{
	FILE *file = fopen(path, "rb");
	int spawnpos_cnt;

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
	spawnpos_cnt = tiles_init_spawn_pos(spawnpos);

	debugf("LOADED TILEMAP '%s': [%d, %d] %d spawn(s)\n",
	       path, tiles_w, tiles_h, spawnpos_cnt);
}

void tiles_render(void)
{
	int x_off = 0;
	int y_off = 0;

	vec2f_t ppos;

	player_get_pos_lerped(&ppos);
	x_off = MIN(ppos.x - ((DISPLAY_WIDTH >> 1) - (TILE_SIZE >> 1)), 0);
	y_off = MIN(ppos.y - ((DISPLAY_HEIGHT >> 1) - (TILE_SIZE >> 1)), 0);

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
