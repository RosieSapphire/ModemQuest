#ifndef _GAME_TILES_H_
#define _GAME_TILES_H_

#include <stdint.h>

#include "engine/config.h"

#define TILE_SIZE 32
#define TILES_X 10
#define TILES_Y 10
#define TILES_DIM (TILES_X * TILES_Y)

enum
{
	TILE_TYPE_EMPTY = 0,
	TILE_TYPE_FLOOR,
	TILE_TYPE_WALL,
};

extern uint8_t tiles[TILES_DIM];

void tiles_init(void);
void tiles_render(void);

#endif /* _GAME_TILES_H_ */
