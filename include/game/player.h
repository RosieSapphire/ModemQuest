#ifndef _GAME_PLAYER_H_
#define _GAME_PLAYER_H_

#include <libdragon.h>

#include "vec2.h"

#define PLAYER_NAME_LEN 20

#define PLAYER_FLAG_TALKING 0x1

#define PLAYER_MOVE_TIMER_MAX 32

enum
{
	PLAYER_DIR_UP,
	PLAYER_DIR_DOWN,
	PLAYER_DIR_LEFT,
	PLAYER_DIR_RIGHT,
	PLAYER_DIR_CNT,
};

typedef struct
{
	char name[PLAYER_NAME_LEN];
	vec2i_t pos;
	vec2f_t pos_lerp_a, pos_lerp_b;
	int dir, move_timer;
	int flags;
} player_t;

extern player_t player;

/* base */
void player_init(const vec2i_t pos);
void player_get_pos_lerped(vec2f_t *v);
void player_render(void);

/* update */
void player_update(const joypad_inputs_t held);
void player_update_moving(const joypad_inputs_t held, vec2i_t *move);
void player_update_collision(const vec2i_t pos_old, const vec2i_t move,
			     vec2i_t *pos_new_out);

#endif /* _GAME_PLAYER_H_ */
