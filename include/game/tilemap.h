#ifndef _GAME_TILEMAP_H_
#define _GAME_TILEMAP_H_

#include "vec2.h"

#define joypad_buttons_t void *
#include "game/npc.h"
#undef joypad_buttons_t

#define TILEMAP_W_MAX 256
#define TILEMAP_H_MAX 256
#define TILEMAP_NPC_MAX 128
#define TILE_SIZE 32

enum {
	TILE_TYPE_PLAYER_SPAWN = 0,
	TILE_TYPE_NPC,
	TILE_TYPE_FLOOR,
	TILE_TYPE_WALL,
	TILE_TYPE_CNT,
};

#define TILE_TYPE_IS_COLLIDABLE(X) \
	(((X) != TILE_TYPE_FLOOR) && ((X) != TILE_TYPE_PLAYER_SPAWN))

typedef struct {
	u8 type;
	u16 col;
} tile_t;

extern u16 tilemap_w, tilemap_h, tilemap_npc_cnt;
extern tile_t tilemap[TILEMAP_H_MAX][TILEMAP_W_MAX];
extern npc_t tilemap_npcs[TILEMAP_NPC_MAX];

void tilemap_load(const char *path, vec2i spawnpos);
void tilemap_render(void);

#endif /* _GAME_TILEMAP_H_ */
