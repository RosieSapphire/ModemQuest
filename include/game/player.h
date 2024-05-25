#ifndef _GAME_PLAYER_H_
#define _GAME_PLAYER_H_

#include <libdragon.h>

#define PLAYER_NAME_LEN 20

typedef struct
{
	char name[PLAYER_NAME_LEN];
	int x, y, move_timer;
	float x_lerp_a, y_lerp_a, x_lerp_b, y_lerp_b;
} player_t;

extern player_t player;

void player_init(const int x, const int y);
void player_update(const joypad_inputs_t held);
void player_render(void);

#endif /* _GAME_PLAYER_H_ */
