#include "util.h"

#include "game/tilemap.h"
#include "game/player.h"

void player_update(const joypad_inputs_t held)
{
	if (player.flags & PLAYER_FLAG_TALKING)
		return;

	int pos_old[2], pos_new[2], move[2];

	VEC2_COPY(pos_old, player.pos);
	player_update_moving(held, move);
	player_update_collision(pos_old, move, pos_new);

	/* change direction */
	int xdelta = pos_new[0] - pos_old[0];
	int ydelta = pos_new[1] - pos_old[1];

	if (ydelta > 0)
		player.dir = PLAYER_DIR_DOWN;
	if (ydelta < 0)
		player.dir = PLAYER_DIR_UP;
	if (xdelta > 0)
		player.dir = PLAYER_DIR_RIGHT;
	if (xdelta < 0)
		player.dir = PLAYER_DIR_LEFT;

	if (player.move_timer < 0)
	{
		VEC2_COPY(player.pos, pos_new);
		VEC2_SCALE(player.pos_lerp_a, pos_old, TILE_SIZE);
		VEC2_SCALE(player.pos_lerp_b, pos_new, TILE_SIZE);
		player.move_timer += PLAYER_MOVE_TIMER_MAX;
	}
}

void player_update_moving(const joypad_inputs_t held, int move[2])
{
	int dpad_x = held.btn.d_right - held.btn.d_left;
	int dpad_y = held.btn.d_down - held.btn.d_up;
	int trying_to_move = (held.stick_x != 0 || held.stick_y != 0) |
		(dpad_x != 0 || dpad_y != 0);

	if (trying_to_move)
		player.move_timer -= (held.btn.z | held.btn.l) + 1;
	else if (player.move_timer > 0)
		player.move_timer--;

	if (player.move_timer >= 0)
	{
		VEC2_ZERO(move);
		return;
	}

	VEC2_SET(move, (held.stick_x > 0) - (held.stick_x < 0) + dpad_x,
		 (held.stick_y < 0) - (held.stick_y > 0) + dpad_y);
}

void player_update_collision(const int pos_old[2], const int move[2],
			     int pos_new_out[2])
{
	int pos_new[2];

	VEC2_ADD(pos_new, pos_old, move);
	for (int i = 0; i < 4; i++)
	{
		/* order: up-right, up-left, down-right, down-left */
		const int poslut[4][2] = {
			{ 1,  1}, {-1,  1}, { 1, -1}, {-1, -1},
		};

		if (pos_new[0] != pos_old[0] + poslut[i][0] ||
		    pos_new[1] != pos_old[1] + poslut[i][1])
			continue;

		if (TILE_TYPE_IS_COLLIDABLE(tilemap[pos_old[1]][pos_old[0] +
		    poslut[i][0]].type))
			pos_new[0] = pos_old[0];

		if (TILE_TYPE_IS_COLLIDABLE(tilemap[pos_old[1] +
		    poslut[i][1]][pos_old[0]].type))
			pos_new[1] = pos_old[1];

		if (TILE_TYPE_IS_COLLIDABLE(tilemap[pos_old[1] +
		    poslut[i][1]][pos_old[0] + poslut[i][0]].type))
		{
			if (ABS(move[0]) > ABS(move[1]))
				pos_new[1] = pos_old[1];
			else
				pos_new[0] = pos_old[0];
		}
	}

	if (TILE_TYPE_IS_COLLIDABLE(tilemap[pos_new[1]][pos_new[0]].type))
	{
		VEC2_COPY(pos_new, pos_old);
		player.move_timer = 0;
	}

	VEC2_COPY(pos_new_out, pos_new);
}
