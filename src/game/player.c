#include <stdio.h>
#include <math.h>

#include "engine/config.h"
#include "engine/util.h"

#include "game/tiles.h"
#include "game/player.h"

#define PLAYER_MOVE_TIMER_MAX 32

player_t player;

void player_init(const int x, const int y)
{
	snprintf(player.name, 20, "Timmy");
	player.x = x;
	player.y = y;
	player.move_timer = 0;
	player.x_lerp_a = x * TILE_SIZE;
	player.y_lerp_a = y * TILE_SIZE;
	player.x_lerp_b = x * TILE_SIZE;
	player.y_lerp_b = y * TILE_SIZE;
	debugf("INITIALIZED PLAYER '%s'\n", player.name);
}

void player_update(const joypad_inputs_t held)
{
	int stick_x = held.stick_x;
	int stick_y = held.stick_y;
	int dpad_x = held.btn.d_right - held.btn.d_left;
	int dpad_y = held.btn.d_down - held.btn.d_up;

	if (stick_x > -STICK_DEADZONE && stick_x < STICK_DEADZONE)
		stick_x = 0;
	if (stick_y > -STICK_DEADZONE && stick_y < STICK_DEADZONE)
		stick_y = 0;

	const int stick_used = stick_x != 0 || stick_y != 0;
	const int dpad_used = dpad_x != 0 || dpad_y != 0;

	if (stick_used | dpad_used)
		player.move_timer -= (held.btn.z | held.btn.l) + 1;
	else if (player.move_timer > 0)
		player.move_timer--;

	if (player.move_timer < 0)
	{
		const int stick_move_x = (stick_x > 0) - (stick_x < 0);
		const int stick_move_y = (stick_y < 0) - (stick_y > 0);
		const int player_move_x = stick_move_x + dpad_x;
		const int player_move_y = stick_move_y + dpad_y;
		const int player_x_old = player.x;
		const int player_y_old = player.y;
		int player_x_new = player_x_old + player_move_x;
		int player_y_new = player_y_old + player_move_y;

		/*
		 * make sure that when we're going diagonally, we're not
		 * phasing through adjacent tiles, and also so we can
		 * slide on walls if we're moving diagonal just a little bit
		 */
		for (int i = 0; i < 4; i++)
		{
			/* order: up-right, up-left, down-right, down-left */
			const int poslut[4][2] = {
				{ 1,  1}, {-1,  1}, { 1, -1}, {-1, -1},
			};

			if (player_x_new != player_x_old + poslut[i][0] ||
			    player_y_new != player_y_old + poslut[i][1])
				continue;

			if (tiles[player_y_old]
			    [player_x_old + poslut[i][0]].type
			    == TILE_TYPE_WALL)
				player_x_new = player_x_old;

			if (tiles[player_y_old + poslut[i][1]]
			    [player_x_old].type
			    == TILE_TYPE_WALL)
				player_y_new = player_y_old;

			if (tiles[player_y_old + poslut[i][1]]
			    [player_x_old + poslut[i][0]].type
			    == TILE_TYPE_WALL)
			{
				if (ABS(player_move_x) > ABS(player_move_y))
					player_y_new = player_y_old;
				else
					player_x_new = player_x_old;
			}
		}

		if (tiles[player_y_new][player_x_new].type == TILE_TYPE_WALL)
		{
			player_x_new = player_x_old;
			player_y_new = player_y_old;
			player.x = player_x_new;
			player.y = player_y_new;
			player.move_timer = 0;
			return;
		}

		player.x = player_x_new;
		player.y = player_y_new;
		player.x_lerp_a = player_x_old * TILE_SIZE;
		player.y_lerp_a = player_y_old * TILE_SIZE;
		player.x_lerp_b = player_x_new * TILE_SIZE;
		player.y_lerp_b = player_y_new * TILE_SIZE;
		player.move_timer += PLAYER_MOVE_TIMER_MAX;
	}
}

void player_get_pos_lerped(float *x, float *y)
{
	const float t = (float)(PLAYER_MOVE_TIMER_MAX - player.move_timer) /
		PLAYER_MOVE_TIMER_MAX;
	*x = roundf(lerpf(player.x_lerp_a, player.x_lerp_b, t));
	*y = roundf(lerpf(player.y_lerp_a, player.y_lerp_b, t));
}

void player_render(void)
{
	const uint16_t col = (0x06 << 11) | (0x11 << 6) |
			     (0x15 << 1) | 1;
	float x, y;

	player_get_pos_lerped(&x, &y);
	x = MAX(x, (DISPLAY_WIDTH >> 1) - (TILE_SIZE >> 1));
	y = MAX(y, (DISPLAY_HEIGHT >> 1) - (TILE_SIZE >> 1));
	rdpq_fill_rect_border(x, y, x + TILE_SIZE, y + TILE_SIZE, col, 2);
}
