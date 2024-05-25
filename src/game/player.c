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
		const int player_x_old = player.x;
		const int player_y_old = player.y;
		int player_x_new = player_x_old + stick_move_x + dpad_x;
		int player_y_new = player_y_old + stick_move_y + dpad_y;

		if (tiles[player_y_new * TILES_X + player_x_new]
		    == TILE_TYPE_WALL)
		{
			player_x_new = player_x_old;
			player_y_new = player_y_old;
			player.x = player_x_new;
			player.y = player_y_new;
			player.move_timer = 0;
			return;
		}

		/*
		 * make sure that when we're going diagonally, we're not
		 * phasing through adjascent tiles
		 */
		for (int i = 0; i < 4; i++)
		{
			/* order: up-right, up-left, down-right, down-left */
			const int poslut[4][2] = {
				{ 1,  1}, {-1,  1}, { 1, -1}, {-1, -1},
			};

			if (player_x_new == player_x_old + poslut[i][0] &&
			    player_y_new == player_y_old + poslut[i][1])
			{
				if (tiles[player_y_old * TILES_X +
				    (player_x_old + poslut[i][0])]
				    == TILE_TYPE_WALL)
					player_x_new = player_x_old;

				if (tiles[(player_y_old + poslut[i][1]) *
				    TILES_X + player_x_old] == TILE_TYPE_WALL)
					player_y_new = player_y_old;
			}
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

void player_render(void)
{
	const float t = (float)(PLAYER_MOVE_TIMER_MAX - player.move_timer) /
		PLAYER_MOVE_TIMER_MAX;
	const int xlerp = roundf(lerpf(player.x_lerp_a, player.x_lerp_b, t));
	const int ylerp = roundf(lerpf(player.y_lerp_a, player.y_lerp_b, t));

	rdpq_fill_rect_border(xlerp, ylerp,
			      xlerp + TILE_SIZE,
			      ylerp + TILE_SIZE,
			      0x06, 0x11, 0x15, 2);
}
