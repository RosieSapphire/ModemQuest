#include <libdragon.h>

#include "input.h"
#include "config.h"
#include "rdpq_ext.h"
#include "util.h"

#include "engine/tilemap.h"
#include "engine/player.h"

#define PLAYER_MOVE_PXLS_PER_SEC 60

player_t player;

void player_init(const vec2i pos)
{
	snprintf(player.name, PLAYER_NAME_MAX_LEN, "%s", "Timmy");
	vec2i_copy(player.pos, pos);
	vec2f_set(player.pos_goal_a, pos[0] * TILE_SIZE_PXLS,
		  pos[1] * TILE_SIZE_PXLS);
	vec2f_copy(player.pos_goal_b, player.pos_goal_a);
	vec2f_copy(player.pos_goal_a_old, player.pos_goal_a);
	vec2f_copy(player.pos_goal_b_old, player.pos_goal_b);
	player.dir = PLAYER_DIR_DOWN;
	player.move_timer = player.move_timer_old = 0.f;
	player.flags = 0;
	debugf("INITIALIZED PLAYER '%s'\n", player.name);
}

void player_get_pos_lerped(vec2f v, const float subtick)
{
	const float move_timer_lerp =
		lerpf(player.move_timer_old, player.move_timer, subtick);
	const float t = (float)(PLAYER_MOVE_TIMER_MAX - move_timer_lerp) /
			PLAYER_MOVE_TIMER_MAX;
	vec2f goal_a_lerp, goal_b_lerp;
	vec2f_lerp(goal_a_lerp, player.pos_goal_a_old, player.pos_goal_a,
		   subtick);
	vec2f_lerp(goal_b_lerp, player.pos_goal_b_old, player.pos_goal_b,
		   subtick);

	vec2f_lerp(v, goal_a_lerp, goal_b_lerp, t);
	vec2f_round(v);
}

static void _player_update_moving(vec2i move, const float dt,
				  const int is_trying_to_move, const vec2i dpad)
{
	if (is_trying_to_move) {
		player.move_timer -=
			((INPUT_GET_BTN(Z, HELD) | INPUT_GET_BTN(L, HELD)) +
			 1) *
			dt * PLAYER_MOVE_PXLS_PER_SEC;
	} else if (player.move_timer > 0) {
		player.move_timer -= dt * PLAYER_MOVE_PXLS_PER_SEC;
	}

	if (player.move_timer > 0) {
		vec2i_zero(move);
		return;
	}

	vec2i_set(move,
		  (INPUT_GET_STICK(X) > 0) - (INPUT_GET_STICK(X) < 0) + dpad[0],
		  (INPUT_GET_STICK(Y) < 0) - (INPUT_GET_STICK(Y) > 0) +
			  dpad[1]);
}

static int _player_update_collision(const vec2i pos_old, const vec2i move,
				    vec2i pos_new_out)
{
	int is_unable_to_move = false;
	vec2i pos_new;

	vec2i_add(pos_new, pos_old, move);

	for (int i = 0; i < 4; i++) {
		/* order: up-right, up-left, down-right, down-left */
		const vec2i poslut[4] = {
			{ 1, 1 },
			{ -1, 1 },
			{ 1, -1 },
			{ -1, -1 },
		};

		if (pos_new[0] != pos_old[0] + poslut[i][0] ||
		    pos_new[1] != pos_old[1] + poslut[i][1]) {
			continue;
		}

		if (TILE_TYPE_IS_COLLIDABLE(
			    tilemap_tiles[pos_old[1]][pos_old[0] + poslut[i][0]]
				    .type)) {
			pos_new[0] = pos_old[0];
		}

		if (TILE_TYPE_IS_COLLIDABLE(
			    tilemap_tiles[pos_old[1] + poslut[i][1]][pos_old[0]]
				    .type)) {
			pos_new[1] = pos_old[1];
		}

		if (TILE_TYPE_IS_COLLIDABLE(
			    tilemap_tiles[pos_old[1] + poslut[i][1]]
					 [pos_old[0] + poslut[i][0]]
						 .type)) {
			if ((int)fabsf((float)move[0]) >
			    (int)fabsf((float)move[1])) {
				pos_new[1] = pos_old[1];
			} else {
				pos_new[0] = pos_old[0];
			}
		}
	}

	if (TILE_TYPE_IS_COLLIDABLE(
		    tilemap_tiles[pos_new[1]][pos_new[0]].type)) {
		vec2i_copy(pos_new, pos_old);
		// debugf("Nope: (%d, %d)\n", move[0], move[1]);
		is_unable_to_move = true;
		player.move_timer = 0.f;
	}

	vec2i_copy(pos_new_out, pos_new);

	return is_unable_to_move;
}

static void _player_update_direction(const vec2i pos_old, const vec2i pos_new,
				     const int is_unable_to_move,
				     const vec2i intended_move)
{
	vec2i delta;

	if (is_unable_to_move) {
		vec2i_copy(delta, intended_move);
	} else {
		vec2i_sub(delta, pos_new, pos_old);
	}

	if (delta[1] > 0)
		player.dir = PLAYER_DIR_DOWN;
	if (delta[1] < 0)
		player.dir = PLAYER_DIR_UP;
	if (delta[0] > 0)
		player.dir = PLAYER_DIR_RIGHT;
	if (delta[0] < 0)
		player.dir = PLAYER_DIR_LEFT;
}

void player_update(const float dt)
{
	/* if we're talking, we can't move */
	if (player.flags & PLAYER_FLAG_IS_TALKING) {
		return;
	}

	vec2i dpad = { INPUT_GET_BTN(DPAD_RIGHT, HELD) -
			       INPUT_GET_BTN(DPAD_LEFT, HELD),
		       INPUT_GET_BTN(DPAD_DOWN, HELD) -
			       INPUT_GET_BTN(DPAD_UP, HELD) };
	int is_trying_to_move = (INPUT_GET_STICK(X) || INPUT_GET_STICK(Y) ||
				 dpad[0] || dpad[1]);
	vec2i pos_old, pos_new, move;

	/* old values */
	vec2i_copy(pos_old, player.pos);
	vec2f_copy(player.pos_goal_a_old, player.pos_goal_a);
	vec2f_copy(player.pos_goal_b_old, player.pos_goal_b);
	player.move_timer_old = player.move_timer;

	_player_update_moving(move, dt, is_trying_to_move, dpad);

	/* next moving step */
	if (player.move_timer >= 0.f) {
		return;
	}

	int is_unable_to_move =
		_player_update_collision(pos_old, move, pos_new);
	_player_update_direction(pos_old, pos_new, is_unable_to_move, move);

	if (is_unable_to_move) {
		return;
	}

	if (is_trying_to_move) {
		vec2i_copy(player.pos, pos_new);
		vec2f_scale(player.pos_goal_a,
			    (const vec2f){ pos_old[0], pos_old[1] },
			    TILE_SIZE_PXLS);
		vec2f_scale(player.pos_goal_b,
			    (const vec2f){ pos_new[0], pos_new[1] },
			    TILE_SIZE_PXLS);
		vec2f_copy(player.pos_goal_a_old, player.pos_goal_a);
		player.move_timer += PLAYER_MOVE_TIMER_MAX;
		player.move_timer_old += PLAYER_MOVE_TIMER_MAX;
		return;
	}

	player.move_timer = 0.f;
	vec2f_copy(player.pos_goal_a, player.pos_goal_b);
}

void player_render(const float subtick)
{
	const uint16_t col = (0x06 << 11) | (0x11 << 6) | (0x15 << 1) | 1;
	vec2f real_pos;

	player_get_pos_lerped(real_pos, subtick);

	// debug goal points a and b
	/*
	const uint16_t col2 = (0x03 << 11) | (0x8 << 6) | (0xA << 1) | 1;
	vec2f goal_a_pos;
	vec2f_copy(goal_a_pos, player.pos_goal_a);

	goal_a_pos[0] = fminf(goal_a_pos[0],
			      (DISPLAY_WIDTH >> 1) - (TILE_SIZE_PXLS >> 1));
	goal_a_pos[1] = fminf(goal_a_pos[1],
			      (DISPLAY_HEIGHT >> 1) - (TILE_SIZE_PXLS >> 1));
	rdpq_fill_rect_border(player.pos_goal_a[0], player.pos_goal_a[1],
			      player.pos_goal_a[0] + TILE_SIZE_PXLS,
			      player.pos_goal_a[1] + TILE_SIZE_PXLS, col2, 2);

	vec2f goal_b_pos;
	vec2f_copy(goal_b_pos, player.pos_goal_b);
	goal_b_pos[0] = fminf(goal_b_pos[0],
			      (DISPLAY_WIDTH >> 1) - (TILE_SIZE_PXLS >> 1));
	goal_b_pos[1] = fminf(goal_b_pos[1],
			      (DISPLAY_HEIGHT >> 1) - (TILE_SIZE_PXLS >> 1));
	rdpq_fill_rect_border(player.pos_goal_b[0], player.pos_goal_b[1],
			      player.pos_goal_b[0] + TILE_SIZE_PXLS,
			      player.pos_goal_b[1] + TILE_SIZE_PXLS, col2, 2);
			      */

	/* TODO: Make the bottom right corner of the map clamp properly */
	if (real_pos[0] > PLAYER_RENDER_POS_X_MAX) {
		real_pos[0] = PLAYER_RENDER_POS_X_MAX;
	}
	if (real_pos[1] > PLAYER_RENDER_POS_Y_MAX) {
		real_pos[1] = PLAYER_RENDER_POS_Y_MAX;
	}
	rdpq_fill_rect_border(real_pos[0], real_pos[1],
			      real_pos[0] + TILE_SIZE_PXLS,
			      real_pos[1] + TILE_SIZE_PXLS, col, 2);

	/* direction */
	const int rects[NUM_PLAYER_DIRS][4] = {
		{
			/* up */
			real_pos[0] + (TILE_SIZE_PXLS >> 1) - 1,
			real_pos[1] + 0,
			real_pos[0] + (TILE_SIZE_PXLS >> 1) + 1,
			real_pos[1] + 6,
		},
		{
			/* down */
			real_pos[0] + (TILE_SIZE_PXLS >> 1) - 1,
			real_pos[1] + TILE_SIZE_PXLS - 6,
			real_pos[0] + (TILE_SIZE_PXLS >> 1) + 1,
			real_pos[1] + TILE_SIZE_PXLS,
		},
		{
			/* left */
			real_pos[0] + 0,
			real_pos[1] + (TILE_SIZE_PXLS >> 1) - 1,
			real_pos[0] + 6,
			real_pos[1] + (TILE_SIZE_PXLS >> 1) + 1,
		},
		{
			/* right */
			real_pos[0] + TILE_SIZE_PXLS - 6,
			real_pos[1] + (TILE_SIZE_PXLS >> 1) - 1,
			real_pos[0] + TILE_SIZE_PXLS,
			real_pos[1] + (TILE_SIZE_PXLS >> 1) + 1,
		},
	};

	rdpq_fill_rectangle(rects[player.dir][0], rects[player.dir][1],
			    rects[player.dir][2], rects[player.dir][3]);
}

void player_terminate(void)
{
}
