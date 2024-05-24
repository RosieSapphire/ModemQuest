#include <libdragon.h>

#include "engine/config.h"
#include "engine/util.h"
#include "engine/fade.h"

#define FADE_SURF_WIDTH 32
#define FADE_SURF_HEIGHT 32
#define FADE_SURF_PIXEL_CNT (DISPLAY_WIDTH * DISPLAY_HEIGHT)

#define FADE_RADIUS_MAX 512
#define FADE_SPEED 16

static uint16_t surf_pixels[FADE_SURF_PIXEL_CNT];
static surface_t surf;

static int fade_state = FADE_STATE_DISABLED;
static int fade_radius = FADE_RADIUS_MAX;

void fade_load_spr(void)
{
	for (int y = 0; y < FADE_SURF_HEIGHT; y++)
	{
		for (int x = 0; x < FADE_SURF_WIDTH; x++)
		{
			const int dir_x = x - (FADE_SURF_WIDTH >> 1);
			const int dir_y = y - (FADE_SURF_HEIGHT >> 1);
			const int dist_sq = dir_x * dir_x + dir_y * dir_y;
			const int ind = y * FADE_SURF_WIDTH + x;

			surf_pixels[ind] = (dist_sq < 16 * 16) * 0xFFFF;
		}
	}
	surf = surface_make_linear(surf_pixels, FMT_RGBA16,
				   FADE_SURF_WIDTH, FADE_SURF_HEIGHT);
	data_cache_writeback_invalidate_all();
}

void fade_state_setup(const int fade_state_new)
{
	const int fade_radius_new[3] = {
		0, 0, 512,
	};

	fade_state = fade_state_new;
	fade_radius = fade_radius_new[fade_state + 1];
}

int fade_update(const int should_switch)
{
	const int fade_radius_new[FADE_STATE_CNT] = {
		fade_radius + FADE_SPEED,
		fade_radius - FADE_SPEED,
	};

	fade_state_setup(fade_state ^ should_switch);
	fade_radius = fade_radius_new[fade_state];
	if (fade_radius < 0)
	{
		fade_radius = 0;
		return (1);
	}

	return (0);
}

void fade_render(void)
{
	if (fade_radius == FADE_RADIUS_MAX ||
	    fade_state == FADE_STATE_DISABLED)
		return;

	const int rect[4] = {
		-(fade_radius >> 1) + (DISPLAY_WIDTH >> 1),
		-(fade_radius >> 1) + (DISPLAY_HEIGHT >> 1),
		 (fade_radius >> 1) + (DISPLAY_WIDTH >> 1),
		 (fade_radius >> 1) + (DISPLAY_HEIGHT >> 1),
	};

	if (!fade_radius)
		goto draw_black;

	rdpq_set_mode_standard();
	rdpq_mode_blender(RDPQ_BLENDER((MEMORY_RGB, IN_ALPHA,
					BLEND_RGB, ZERO)));
	rdpq_mode_filter(FILTER_BILINEAR);
	rdpq_tex_upload(TILE0, &surf, NULL);
	rdpq_texture_rectangle_scaled(TILE0,
				      rect[0], rect[1],  rect[2], rect[3],
				      0, 0, FADE_SURF_WIDTH, FADE_SURF_HEIGHT);

	rdpq_set_mode_fill(RGBA16(0, 0, 0, 1));
	rdpq_fill_rectangle(0, 0, rect[0], DISPLAY_HEIGHT);
	rdpq_fill_rectangle(rect[2], 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	rdpq_fill_rectangle(rect[0], 0, rect[2], rect[1]);
	rdpq_fill_rectangle(rect[0], rect[3], rect[2], DISPLAY_HEIGHT);

	return;

draw_black:
	rdpq_clear(RGBA16(0, 0, 0, 1));
}
