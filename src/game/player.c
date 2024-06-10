#include <stdio.h>
#include <math.h>

#include "config.h"
#include "util.h"

#include "game/tilemap.h"
#include "game/player.h"

player_t player;

void player_init(const vec2i pos)
{
	snprintf(player.name, 20, "Timmy");
	VEC2_COPY(player.pos, pos);
	VEC2_SET(player.pos_lerp_a, pos[0] * TILE_SIZE, pos[1] * TILE_SIZE);
	VEC2_SET(player.pos_lerp_b, pos[0] * TILE_SIZE, pos[1] * TILE_SIZE);
	player.dir = PLAYER_DIR_DOWN;
	player.move_timer = 0;
	player.flags = 0;
	debugf("INITIALIZED PLAYER '%s'\n", player.name);
}

void player_get_pos_lerped(float v[2])
{
	const float t = (float)(PLAYER_MOVE_TIMER_MAX - player.move_timer) /
		PLAYER_MOVE_TIMER_MAX;

	VEC2_LERP(v, player.pos_lerp_a, player.pos_lerp_b, t);
	VEC2_ROUND(v);
}

void player_render(void)
{
	const u16 col = (0x06 << 11) | (0x11 << 6) |
			     (0x15 << 1) | 1;
	float pos[2];

	player_get_pos_lerped(pos);
	pos[0] = MAX(pos[0], (DSP_WID >> 1) - (TILE_SIZE >> 1));
	pos[1] = MAX(pos[1], (DSP_HEI >> 1) - (TILE_SIZE >> 1));
	rdpq_fill_rect_border(pos[0], pos[1],
			      pos[0] + TILE_SIZE, pos[1] + TILE_SIZE, col, 2);

	/* direction */
	const int rects[PLAYER_DIR_CNT][4] = {
		{ /* up */
			pos[0] + (TILE_SIZE >> 1) - 1,
			pos[1] + 0,
			pos[0] + (TILE_SIZE >> 1) + 1,
			pos[1] + 6,
		},
		{ /* down */
			pos[0] + (TILE_SIZE >> 1) - 1,
			pos[1] + TILE_SIZE - 6,
			pos[0] + (TILE_SIZE >> 1) + 1,
			pos[1] + TILE_SIZE,
		},
		{ /* left */
			pos[0] + 0,
			pos[1] + (TILE_SIZE >> 1) - 1,
			pos[0] + 6,
			pos[1] + (TILE_SIZE >> 1) + 1,
		},
		{ /* right */
			pos[0] + TILE_SIZE - 6,
			pos[1] + (TILE_SIZE >> 1) - 1,
			pos[0] + TILE_SIZE,
			pos[1] + (TILE_SIZE >> 1) + 1,
		},
	};

	rdpq_fill_rectangle(rects[player.dir][0], rects[player.dir][1],
			    rects[player.dir][2], rects[player.dir][3]);
}
