#ifndef _ENGINE_PLAYER_H_
#define _ENGINE_PLAYER_H_

#include "vec2.h"

#define PLAYER_NAME_MAX_LEN 20
#define PLAYER_MOVE_TIMER_MAX 32

enum { PLAYER_FLAG_IS_TALKING = (1 << 0) };

enum {
	PLAYER_DIR_UP,
	PLAYER_DIR_DOWN,
	PLAYER_DIR_LEFT,
	PLAYER_DIR_RIGHT,
	NUM_PLAYER_DIRS
};

typedef struct {
	char name[PLAYER_NAME_MAX_LEN];
	vec2i pos;
	vec2f pos_goal_a, pos_goal_a_old, pos_goal_b, pos_goal_b_old;
	int dir;
	float move_timer, move_timer_old;
	int flags;
} player_t;

extern player_t player;

void player_init(const vec2i pos);
void player_get_pos_lerped(vec2f v, const float subtick);
void player_update(const float dt);
void player_render(const float subtick);
void player_terminate(void);

#endif /* _ENGINE_PLAYER_H_ */
