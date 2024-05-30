#include <libdragon.h>

#include "engine/config.h"
#include "engine/util.h"
#include "engine/fade.h"

#define FADE_SURF_WIDTH 64
#define FADE_SURF_HEIGHT 64
#define FADE_SURF_PIXEL_CNT (DISPLAY_WIDTH * DISPLAY_HEIGHT)

#define FADE_RADIUS_MAX 420
#define FADE_SPEED 16

static sprite_t *spr_circle;

static int fade_state = FADE_STATE_DISABLED;
static int fade_radius = FADE_RADIUS_MAX;

void fade_load_spr(void)
{
	spr_circle = sprite_load("rom:/fade_circle.ia4.sprite");
}

void fade_state_setup(const int fade_state_new)
{
	const int fade_radius_new[3] = {
		0, 0, FADE_RADIUS_MAX,
	};

	fade_state = fade_state_new;
	fade_radius = fade_radius_new[fade_state + 1];
}

int fade_update(const int should_switch)
{
	if (fade_state == FADE_STATE_DISABLED)
		return (0);

	const int fade_radius_new[FADE_STATE_CNT] = {
		fade_radius + FADE_SPEED,
		fade_radius - FADE_SPEED,
	};

	fade_state_setup(fade_state ^ should_switch);
	fade_radius = MAX(fade_radius_new[fade_state], FADE_RADIUS_MAX);
	if (fade_radius < 0)
	{
		fade_radius = 0;
		return (1);
	}

	debugf("%d\n", fade_radius);

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
	rdpq_sprite_upload(TILE0, spr_circle, NULL);
	rdpq_mode_dithering(DITHER_BAYER_NONE);
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
