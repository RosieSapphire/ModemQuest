#include <libdragon.h>

#include "input.h"
#include "util.h"
#include "vector.h"
#include "config.h"
#include "font.h"

#include "game/fade_transition.h"
#include "game/title.h"

#define BG_POS_MOVE_PXLS_PER_SEC 60.f
#define LOGO_POS_X 60
#define LOGO_POS_Y 25
#define CREDITS_POS_X (0)
#define CREDITS_POS_Y (DISPLAY_HEIGHT - 42)

static sprite_t *logo_spr = NULL;
static sprite_t *traces_spr = NULL;
static sprite_t *gradient_spr = NULL;

static float timer = 0.f, timer_old = 0.f;
static vec2f bg_pos = { 0.f, 0.f }, bg_pos_old = { 0.f, 0.f };

void title_init(void)
{
	fade_transition_set(FADE_TRANSITION_IN);

	logo_spr = sprite_load("rom:/title-logo.ci8.sprite");
	traces_spr = sprite_load("rom:/title-traces.ia4.sprite");
	gradient_spr = sprite_load("rom:/title-gradient.rgba16.sprite");

	timer = timer_old = 0.f;
	vec2f_zero(bg_pos);
	vec2f_zero(bg_pos_old);
}

scene_index_t title_update(const float dt)
{
	/* check for exit */
	if (fade_transition_update(INPUT_GET_BTN(START, PRESSED), dt)) {
		return SCENE_INDEX_TESTAREA;
	}

	/* old values */
	timer_old = timer;
	vec2f_copy(bg_pos_old, bg_pos);

	/* bg moving */
	bg_pos[0] -= dt * BG_POS_MOVE_PXLS_PER_SEC;
	while (bg_pos[0] <= -256.f) {
		bg_pos[0] += 256.f;
		bg_pos_old[0] += 256.f;
	}
	bg_pos[1] -= dt * BG_POS_MOVE_PXLS_PER_SEC;
	while (bg_pos[1] <= -256.f) {
		bg_pos[1] += 256.f;
		bg_pos_old[1] += 256.f;
	}

	timer += dt;

	return SCENE_INDEX_TITLE;
}

void title_render(const float subtick)
{
	const float timer_lerp = lerpf(timer_old, timer, subtick);
	vec2f bg_pos_lerp;
	vec2f_lerp(bg_pos_lerp, bg_pos_old, bg_pos, subtick);

	rdpq_set_mode_standard();
	rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);

	/* GRADIENT */
	const surface_t gradient_surf = sprite_get_pixels(gradient_spr);

	rdpq_mode_dithering(DITHER_BAYER_NONE);
	rdpq_tex_upload(TILE0, &gradient_surf,
			&(const rdpq_texparms_t){ .s.repeats = REPEAT_INFINITE,
						  .s.mirror = MIRROR_REPEAT });
	rdpq_texture_rectangle(TILE0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT,
			       timer_lerp * 512.f, 0);

	/* TRACES */
	const float traces_fade = (cosf(timer_lerp * PI) + 1.f) * .5f;
	const color_t traces_color =
		RGBA16(lerpf(.588f, .878f, traces_fade) * 31,
		       lerpf(.263f, .431f, traces_fade) * 31,
		       lerpf(.094f, .212f, traces_fade) * 31, 0x1);

	rdpq_mode_blender(
		RDPQ_BLENDER((IN_RGB, IN_ALPHA, MEMORY_RGB, INV_MUX_ALPHA)));
	rdpq_set_prim_color(traces_color);

	for (int i = 0; i < 9; i++) {
		int x = (int)roundf(bg_pos_lerp[0] + 256.f * (i % 3));
		int y = (int)roundf(bg_pos_lerp[1] + 256.f * (i / 3));

		if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) {
			continue;
		}

		rdpq_sprite_blit(traces_spr, x, y, NULL);
	}

	/* LOGO */
	rdpq_set_prim_color(color_from_packed16(0xFFFF));
	rdpq_sprite_blit(logo_spr, LOGO_POS_X, LOGO_POS_Y, NULL);

	/* CREDITS */
	font_printf(CREDITS_POS_X, CREDITS_POS_Y,
		    &(const rdpq_textparms_t){ .align = ALIGN_CENTER,
					       .width = DISPLAY_WIDTH },
		    "A Homebrew N64 Original by\n"
		    "Aeryk Ressler & Rosie Sapphire");

	fade_transition_render(subtick);
}

void title_free(void)
{
	sprite_free(gradient_spr);
	sprite_free(traces_spr);
	sprite_free(logo_spr);

	gradient_spr = NULL;
	traces_spr = NULL;
	logo_spr = NULL;
}
