#include "game/tiles.h"
#include "game/player.h"

void player_update(const joypad_inputs_t held)
{
	if (player.flags & PLAYER_FLAG_TALKING)
		return;

	vec2i_t pos_old, pos_new, move;

	pos_old = player.pos;
	player_update_moving(held, &move);
	player_update_collision(pos_old, move, &pos_new);

	/* change direction */
	int xdelta = pos_new.x - pos_old.x;
	int ydelta = pos_new.y - pos_old.y;

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
		player.pos = pos_new;
		player.pos_lerp_a = VEC2F_SCALE(pos_old, TILE_SIZE);
		player.pos_lerp_b = VEC2F_SCALE(pos_new, TILE_SIZE);
		player.move_timer += PLAYER_MOVE_TIMER_MAX;
	}
}

void player_update_moving(const joypad_inputs_t held, vec2i_t *move)
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
		*move = VEC2I(0, 0);
		return;
	}

	*move = VEC2I((held.stick_x > 0) - (held.stick_x < 0) + dpad_x,
		      (held.stick_y < 0) - (held.stick_y > 0) + dpad_y);
}

void player_update_collision(const vec2i_t pos_old, const vec2i_t move,
			     vec2i_t *pos_new_out)
{
	vec2i_t pos_new = VEC2I_ADD(pos_old, move);

	for (int i = 0; i < 4; i++)
	{
		/* order: up-right, up-left, down-right, down-left */
		const int poslut[4][2] = {
			{ 1,  1}, {-1,  1}, { 1, -1}, {-1, -1},
		};

		if (pos_new.x != pos_old.x + poslut[i][0] ||
		    pos_new.y != pos_old.y + poslut[i][1])
			continue;

		if (tiles[pos_old.y][pos_old.x +
		    poslut[i][0]].type == TILE_TYPE_WALL)
			pos_new.x = pos_old.x;

		if (tiles[pos_old.y + poslut[i][1]][pos_old.x].type
		    == TILE_TYPE_WALL)
			pos_new.y = pos_old.y;

		if (tiles[pos_old.y + poslut[i][1]][pos_old.x +
		    poslut[i][0]].type == TILE_TYPE_WALL)
		{
			if (ABS(move.x) > ABS(move.y))
				pos_new.y = pos_old.y;
			else
				pos_new.x = pos_old.x;
		}
	}

	if (tiles[pos_new.y][pos_new.x].type == TILE_TYPE_WALL)
	{
		pos_new = pos_old;
		player.move_timer = 0;
	}

	*pos_new_out = pos_new;
}
