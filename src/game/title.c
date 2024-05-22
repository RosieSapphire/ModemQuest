#include <libdragon.h>

#include "engine/n64.h"
#include "engine/util.h"
#include "engine/config.h"
#include "engine/font.h"
#include "engine/fade.h"

#include "game/title.h"

/* sprites */
static sprite_t *spr_logo;
static sprite_t *spr_traces;
static sprite_t *spr_gradient;

/* variables */
static int bg_pos[2];

void title_init(void)
{
	/* sprites */
	spr_logo = sprite_load("rom:/title_logo.ci8.sprite");
	spr_traces = sprite_load("rom:/title_traces.ia4.sprite");
	spr_gradient = sprite_load("rom:/title_gradient.rgba16.sprite");

	/* variables */
	bg_pos[0] = 0;
	bg_pos[1] = 0;

	fade_init_vals(FADE_STATE_IN, 0);
}

void title_update(void)
{
	joypad_buttons_t pressed;

	joypad_poll();
	pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);

	/* bg moving */
	bg_pos[0]--;
	if (bg_pos[0] <= -256)
		bg_pos[0] += 256;

	bg_pos[1]--;
	if (bg_pos[1] <= -256)
		bg_pos[1] += 256;

	fade_update(FADE_PIXELS_PER_FRAME_DEFAULT, pressed.start);
}

void title_render(void)
{
	/* background */
	const float time_now = (float)n64_ticks_now / TICKS_PER_SECOND;
	const surface_t bg_surf = sprite_get_pixels(spr_gradient);

	rdpq_clear(RGBA16(0, 0, 0, 1));
	rdpq_set_mode_standard();
	rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);
	rdpq_mode_dithering(DITHER_BAYER_NONE);
	rdpq_mode_antialias(AA_NONE);
	rdpq_tex_upload(TILE0, &bg_surf, &(const rdpq_texparms_t){
			.s.repeats = REPEAT_INFINITE,
			.s.mirror = MIRROR_REPEAT,
			});
	rdpq_texture_rectangle(TILE0, 0, 0, DISPLAY_WIDTH,
			       DISPLAY_HEIGHT, time_now * 512, 0);
	const float traces_fade = (cosf(time_now * 3.14159f) + 1.0f) * 0.5f;
	const color_t traces_color =
		RGBA16(
			lerpf(0.588f, 0.878f, traces_fade) * 31,
			lerpf(0.263f, 0.431f, traces_fade) * 31,
			lerpf(0.094f, 0.212f, traces_fade) * 31, 0x1
		);

	rdpq_mode_alphacompare(1);
	rdpq_set_prim_color(traces_color);

	for (int i = 0; i < 3 * 3; i++)
	{
		int x = bg_pos[0] + 256 * (i % 3);
		int y = bg_pos[1] + 256 * (i / 3);

		if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT)
			continue;

		rdpq_sprite_blit(spr_traces, x, y, NULL);
	}

	/* title text */
	rdpq_set_prim_color(RGBA16(0x1F, 0x1F, 0x1F, 0x1));
	rdpq_sprite_blit(spr_logo, 60, 25, NULL);

	/* credits 'n shit */
	rdpq_set_mode_standard();
	font_printf(0, DISPLAY_HEIGHT - 42, &(const rdpq_textparms_t) {
				.align = ALIGN_CENTER,
				.width = DISPLAY_WIDTH,
			}, "A Homebrew N64 Original by\n"
			"Aeryk Ressler & Rosie Sapphire");

	fade_render();
}

void title_terminate(void)
{
	/* sprites */
	sprite_free(spr_gradient);
	sprite_free(spr_traces);
	sprite_free(spr_logo);
}
