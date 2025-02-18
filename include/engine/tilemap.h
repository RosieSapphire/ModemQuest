#ifndef _ENGINE_TILEMAP_H_
#define _ENGINE_TILEMAP_H_

#ifndef IS_USING_MQME
#include "vector.h"

#include "engine/npc.h"
#include "engine/door.h"
#else /* IS_USING_MQME */
#include <cglm/cglm.h>

#include "../../../include/engine/npc.h"
#include "../../../include/engine/door.h"
#endif /* IS_USING_MQME */

#define TILEMAP_WIDTH_MAX 256
#define TILEMAP_HEIGHT_MAX 256
#define TILEMAP_MAP_INDEX_MAX 255

#define TILE_SIZE_PXLS 32

enum {
	TILE_TYPE_PLAYER_SPAWN,
	TILE_TYPE_NPC,
	TILE_TYPE_FLOOR,
	TILE_TYPE_WALL,
	TILE_TYPE_DOOR,
	TILE_TYPE_COUNT
};

#define TILE_TYPE_IS_COLLIDABLE(X) \
	(((X) != TILE_TYPE_FLOOR) && ((X) != TILE_TYPE_PLAYER_SPAWN))

struct tile {
	u8 type;
	u16 color;
};

struct tilemap {
	u16 map_index;
	u8 width;
	u8 height;
	u8 npc_count;
	u8 door_count;
	struct tile tiles[TILEMAP_HEIGHT_MAX][TILEMAP_WIDTH_MAX];
	struct npc *npcs;
	struct door *doors;
};

extern struct tilemap tilemap;

#ifndef IS_USING_MQME
void tilemap_init(const char *path, vec2s spawn_pos);
void tilemap_update(void);
void tilemap_render(const f32 subtick);
void tilemap_render_npc_dialogue_boxes(void);
void tilemap_free(void);
#endif /* IS_USING_MQME */

#endif /* _ENGINE_TILEMAP_H_ */
