#include "util.h"
#include "config.h"
#include "vec2.h"

#include "game/tilemap.h"
#include "game/player.h"

u16 tilemap_w, tilemap_h, tilemap_npc_cnt;
tile_t tilemap[TILEMAP_H_MAX][TILEMAP_W_MAX];
npc_t tilemap_npcs[TILEMAP_NPC_MAX];

static int tilemap_init_spawn_pos(int *spawnpos)
{
	/* count player spawns */
	int *spawnposs = malloc(0);
	int spawn_cnt = 0;

	for (int y = 0; y < tilemap_h; y++)
	{
		for (int x = 0; x < tilemap_w; x++)
		{
			if (tilemap[y][x].type != TILE_TYPE_PLAYER_SPAWN)
				continue;

			spawnposs = realloc(spawnposs, sizeof(int) *
					    ++spawn_cnt * 2);
			VEC2_SET((spawnposs + (spawn_cnt - 1) * 2), x, y);
		}
	}

	assertf(spawn_cnt, "ERROR: Tilemap doesn't have any player spawns\n");

	/* choose which one to spawn at */
	VEC2_COPY(spawnpos, (spawnposs + ((rand() % spawn_cnt) * 2)));
	free(spawnposs);

	return (spawn_cnt);
}

void tilemap_load(const char *path, vec2i spawnpos)
{
	FILE *file = fopen(path, "rb");
	int spawnpos_cnt;

	assertf(file, "File loading fucked up '%s'\n", path);
	fread(&tilemap_w, 2, 1, file);
	fread(&tilemap_h, 2, 1, file);
	fread(&tilemap_npc_cnt, 2, 1, file);
	for (int y = 0; y < tilemap_h; y++)
	{
		for (int x = 0; x < tilemap_w; x++)
		{
			tile_t *t = tilemap[y] + x;

			fread(&t->type, 1, 1, file);
			fread(&t->col, 2, 1, file);
		}
	}
	for (int i = 0; i < tilemap_npc_cnt; i++)
	{
		npc_t *n = tilemap_npcs + i;

		fread(n->name, 1, NPC_NAME_MAX, file);
		fread(n->pos, 2, 2, file);
		fread(&n->dialogue_line_cnt, 2, 1, file);
		for (int j = 0; j < n->dialogue_line_cnt; j++)
		{
			dialogue_line_t *dl = n->dialogue + j;

			fread(dl->speaker, 1, NPC_NAME_MAX, file);
			fread(dl->line, 1, DIALOGUE_LINE_MAX, file);
		}

		n->state = NPC_STATE_IDLE;
		n->dialogue_cur = -1;
		n->dialogue_char_cur = 0;
	}
	fclose(file);
	spawnpos_cnt = tilemap_init_spawn_pos(spawnpos);
	debugf("LOADED TILEMAP '%s': [%d, %d] %d spawn(s), %d npc(s)\n",
	       path, tilemap_w, tilemap_h, spawnpos_cnt, tilemap_npc_cnt);
}

void tilemap_render(void)
{
	int x_off = 0;
	int y_off = 0;

	float ppos[2];

	player_get_pos_lerped(ppos);
	x_off = MIN(ppos[0] - ((DSP_WID >> 1) - (TILE_SIZE >> 1)), 0);
	y_off = MIN(ppos[1] - ((DSP_HEI >> 1) - (TILE_SIZE >> 1)), 0);

	for (int y = 0; y < tilemap_h; y++)
	{
		for (int x = 0; x < tilemap_w; x++)
		{
			int xo = (x * TILE_SIZE) - x_off;
			int yo = (y * TILE_SIZE) - y_off;

			rdpq_fill_rect_border(xo, yo, xo + TILE_SIZE,
					      yo + TILE_SIZE,
					      tilemap[y][x].col, 2);
		}
	}
}
