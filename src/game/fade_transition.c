#include <libdragon.h>

#include "config.h"
#include "util.h"

#include "game/fade_transition.h"

#define FADE_RADIUS_MAX 420.f
#define FADE_SPEED_PXLS_PER_SEC 960

#define FADE_SURF_DIM 64

static sprite_t *circle_spr = NULL;

static int fade_transition_state = FADE_TRANSITION_DISABLED;
static float fade_transition_radius = FADE_RADIUS_MAX;
static float fade_transition_radius_old = FADE_RADIUS_MAX;

static int is_exiting_current_scene = false;

void fade_transition_init(void)
{
	circle_spr = sprite_load("rom:/fade-circle.ia4.sprite");
	fade_transition_state = FADE_TRANSITION_DISABLED;
	fade_transition_radius = fade_transition_radius_old = 0.f;
	is_exiting_current_scene = false;
}

void fade_transition_set(const int fade_state_new)
{
	const float fade_radius_new[3] = { 0.f, 0.f, FADE_RADIUS_MAX };

	fade_transition_state = fade_state_new;
	fade_transition_radius = fade_radius_new[fade_transition_state + 1];
}

int fade_transition_update(const int attempting_exit, const float dt)
{
	const int exit_condition = attempting_exit & ~is_exiting_current_scene;

	is_exiting_current_scene ^= exit_condition;

	/* old values */
	fade_transition_radius_old = fade_transition_radius;

	if (exit_condition && !is_exiting_current_scene) {
		is_exiting_current_scene = true;
	}

	if (fade_transition_state == FADE_TRANSITION_DISABLED) {
		return 0;
	}

	const float fade_radius_new[NUM_FADE_TRANSITIONS] = {
		fade_transition_radius + (FADE_SPEED_PXLS_PER_SEC * dt),
		fade_transition_radius - (FADE_SPEED_PXLS_PER_SEC * dt),
	};

	fade_transition_set(fade_transition_state ^ exit_condition);
	fade_transition_radius =
		fminf(fade_radius_new[fade_transition_state], FADE_RADIUS_MAX);
	if (fade_transition_radius <= 0.f) {
		fade_transition_radius = 0.f;
		is_exiting_current_scene = false;
		return 1;
	}

	return 0;
}

void fade_transition_render(const float subtick)
{
	const float radius_lerp = lerpf(fade_transition_radius_old,
					fade_transition_radius, subtick);

	if (radius_lerp == FADE_RADIUS_MAX ||
	    fade_transition_state == FADE_TRANSITION_DISABLED) {
		return;
	}

	const int double_radius_rounded = (int)roundf(radius_lerp) >> 1;
	const int rect[4] = { -double_radius_rounded + (DISPLAY_WIDTH >> 1),
			      -double_radius_rounded + (DISPLAY_HEIGHT >> 1),
			      double_radius_rounded + (DISPLAY_WIDTH >> 1),
			      double_radius_rounded + (DISPLAY_HEIGHT >> 1) };

	if (!radius_lerp) {
		rdpq_clear(color_from_packed16(0x0));
		return;
	}

	/* CENTER PORTION */
	rdpq_set_mode_standard();
	rdpq_mode_blender(
		RDPQ_BLENDER((MEMORY_RGB, IN_ALPHA, BLEND_RGB, ZERO)));
	rdpq_mode_filter(FILTER_BILINEAR);
	rdpq_sprite_upload(TILE0, circle_spr, NULL);
	rdpq_mode_dithering(DITHER_BAYER_BAYER);
	rdpq_texture_rectangle_scaled(TILE0, rect[0], rect[1], rect[2], rect[3],
				      0, 0, FADE_SURF_DIM, FADE_SURF_DIM);

	/* OUTER BORDERS */
	rdpq_set_mode_fill(color_from_packed16(0x0));
	rdpq_fill_rectangle(0, 0, rect[0], DISPLAY_HEIGHT);
	rdpq_fill_rectangle(rect[2], 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	rdpq_fill_rectangle(rect[0], 0, rect[2], rect[1]);
	rdpq_fill_rectangle(rect[0], rect[3], rect[2], DISPLAY_HEIGHT);
}

void fade_transition_free(void)
{
	sprite_free(circle_spr);

	circle_spr = NULL;
}
