#ifndef _GAME_TILEMAP_H_
#define _GAME_TILEMAP_H_

#include "vec2.h"

#define TILEMAP_W_MAX 256
#define TILEMAP_H_MAX 256
#define TILE_SIZE 32

enum
{
	TILE_TYPE_PLAYER_SPAWN = 0,
	TILE_TYPE_FLOOR,
	TILE_TYPE_WALL,
	TILE_TYPE_CNT,
};

typedef struct
{
	u8 type;
	u16 col;
} tile_t;

extern u16 tilemap_w, tilemap_h;
extern tile_t tilemap[TILEMAP_H_MAX][TILEMAP_W_MAX];

void tilemap_init(const char *path, vec2i spawnpos);
void tilemap_render(void);

#endif /* _GAME_TILEMAP_H_ */
