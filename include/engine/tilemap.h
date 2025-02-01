#ifndef _ENGINE_TILEMAP_H_
#define _ENGINE_TILEMAP_H_

#include <stdint.h>

#include "vec2.h"

#include "engine/npc.h"

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

extern uint16_t tilemap_width, tilemap_height, tilemap_num_npcs;
extern tile_t tilemap_tiles[TILEMAP_HEIGHT_MAX][TILEMAP_WIDTH_MAX];
extern npc_t tilemap_npcs[TILEMAP_NUM_NPCS_MAX];

void tilemap_init(const char *path, vec2i spawn_pos);
void tilemap_update(void);
void tilemap_render(const float subtick);
void tilemap_render_npc_dialogue_boxes(void);
void tilemap_terminate(void);

#endif /* _ENGINE_TILEMAP_H_ */
