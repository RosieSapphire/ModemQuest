#ifndef _ENGINE_TILEMAP_H_
#define _ENGINE_TILEMAP_H_

#include <stdint.h>

#include "vec2.h"

#define TILEMAP_WIDTH_MAX 256
#define TILEMAP_HEIGHT_MAX 256
#define TILEMAP_NUM_NPCS_MAX 128
#define TILE_SIZE_PXLS 32

enum {
	TILE_TYPE_PLAYER_SPAWN,
	TILE_TYPE_NPC,
	TILE_TYPE_FLOOR,
	TILE_TYPE_WALL,
	NUM_TILE_TYPES
};

#define TILE_TYPE_IS_COLLIDABLE(X) \
	(((X) != TILE_TYPE_FLOOR) && ((X) != TILE_TYPE_PLAYER_SPAWN))

typedef struct {
	uint8_t type;
	uint16_t col;
} tile_t;

void tilemap_init(const char *path, vec2i spawn_pos);
void tilemap_update(void);
void tilemap_render(const float subtick);
void tilemap_render_npc_dialogue_boxes(void);
tile_t *tilemap_get_tile(const int x, const int y);
void tilemap_terminate(void);

#endif /* _ENGINE_TILEMAP_H_ */
