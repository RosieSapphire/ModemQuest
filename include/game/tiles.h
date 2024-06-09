#ifndef _GAME_TILES_H_
#define _GAME_TILES_H_

#include "types.h"

#define TILES_W_MAX 256
#define TILES_H_MAX 256
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

extern u16 tiles_w, tiles_h;
extern tile_t tiles[TILES_H_MAX][TILES_W_MAX];

void tiles_init(const char *path, int *spawnpos);
void tiles_render(void);

#endif /* _GAME_TILES_H_ */
