#ifndef _GAME_TILES_H_
#define _GAME_TILES_H_

#include <stdint.h>

#include "engine/config.h"

#define TILE_SIZE 32
#define TILES_W_MAX 256
#define TILES_H_MAX 256

enum
{
	TILE_TYPE_PLAYER_SPAWN = 0,
	TILE_TYPE_FLOOR,
	TILE_TYPE_WALL,
};

extern uint16_t tiles_w, tiles_h;
extern uint8_t tiles[TILES_H_MAX][TILES_W_MAX];

void tiles_init(const char *path, int *player_spawn_x, int *player_spawn_y);
void tiles_render(void);

#endif /* _GAME_TILES_H_ */
