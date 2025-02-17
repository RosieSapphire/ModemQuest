#ifndef _ENGINE_PLAYER_H_
#define _ENGINE_PLAYER_H_

#include "vector.h"

#define PLAYER_NAME_MAX_LEN 20
#define PLAYER_MOVE_TIMER_MAX 32

#define PLAYER_RENDER_POS_X_MAX ((DISPLAY_WIDTH >> 1) - (TILE_SIZE_PXLS >> 1))
#define PLAYER_RENDER_POS_Y_MAX ((DISPLAY_HEIGHT >> 1) - (TILE_SIZE_PXLS >> 1))

enum { PLAYER_FLAG_IS_TALKING = (1 << 0) };

enum {
	PLAYER_DIR_UP,
	PLAYER_DIR_DOWN,
	PLAYER_DIR_LEFT,
	PLAYER_DIR_RIGHT,
	PLAYER_DIR_COUNT
};

typedef struct {
	char name[PLAYER_NAME_MAX_LEN];
	vec2s pos;
	vec2f pos_goal_a, pos_goal_a_old, pos_goal_b, pos_goal_b_old;
	s8 dir;
	f32 move_timer, move_timer_old;
	u8 flags;
} player_t;

extern player_t player;

void player_init(const vec2s pos);
void player_get_pos_lerped(vec2f v, const float subtick);
void player_update(const float dt);
void player_render(const float subtick);
void player_free(void);

#endif /* _ENGINE_PLAYER_H_ */
