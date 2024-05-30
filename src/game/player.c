#include <stdio.h>
#include <math.h>

#include "engine/config.h"
#include "engine/util.h"

#include "game/tiles.h"
#include "game/player.h"

player_t player;

void player_init(const vec2i_t pos)
{
	snprintf(player.name, 20, "Timmy");
	player.pos = pos;
	player.pos_lerp_a = VEC2F(pos.x * TILE_SIZE, pos.y * TILE_SIZE);
	player.pos_lerp_b = VEC2F(pos.x * TILE_SIZE, pos.y * TILE_SIZE);
	player.dir = PLAYER_DIR_DOWN;
	player.move_timer = 0;
	player.flags = 0;
	debugf("INITIALIZED PLAYER '%s'\n", player.name);
}

void player_get_pos_lerped(vec2f_t *v)
{
	const float t = (float)(PLAYER_MOVE_TIMER_MAX - player.move_timer) /
		PLAYER_MOVE_TIMER_MAX;
	*v = VEC2F_ROUND(VEC2F_LERP(player.pos_lerp_a, player.pos_lerp_b, t));
}

void player_render(void)
{
	const uint16_t col = (0x06 << 11) | (0x11 << 6) |
			     (0x15 << 1) | 1;
	vec2f_t pos;

	player_get_pos_lerped(&pos);
	pos.x = MAX(pos.x, (DISPLAY_WIDTH >> 1) - (TILE_SIZE >> 1));
	pos.y = MAX(pos.y, (DISPLAY_HEIGHT >> 1) - (TILE_SIZE >> 1));
	rdpq_fill_rect_border(pos.x, pos.y,
			      pos.x + TILE_SIZE, pos.y + TILE_SIZE, col, 2);

	/* direction */
	const int rects[PLAYER_DIR_CNT][4] = {
		{ /* up */
			pos.x + (TILE_SIZE >> 1) - 1,
			pos.y + 0,
			pos.x + (TILE_SIZE >> 1) + 1,
			pos.y + 6,
		},
		{ /* down */
			pos.x + (TILE_SIZE >> 1) - 1,
			pos.y + TILE_SIZE - 6,
			pos.x + (TILE_SIZE >> 1) + 1,
			pos.y + TILE_SIZE,
		},
		{ /* left */
			pos.x + 0,
			pos.y + (TILE_SIZE >> 1) - 1,
			pos.x + 6,
			pos.y + (TILE_SIZE >> 1) + 1,
		},
		{ /* right */
			pos.x + TILE_SIZE - 6,
			pos.y + (TILE_SIZE >> 1) - 1,
			pos.x + TILE_SIZE,
			pos.y + (TILE_SIZE >> 1) + 1,
		},
	};

	rdpq_fill_rectangle(rects[player.dir][0], rects[player.dir][1],
			    rects[player.dir][2], rects[player.dir][3]);
}
