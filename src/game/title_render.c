#include <libdragon.h>
#include <t3d/t3dmath.h>

#include "config.h"
#include "util.h"

#include "engine/fade.h"
#include "engine/font.h"
#include "engine/n64.h"

#include "game/title.h"

void title_render(void)
{
	const float time_now = (float)n64_ticks_now / TICKS_PER_SECOND;

	title_background_render(time_now);
	title_logo_render();
	title_credits_render();
	fade_render();
}

void title_background_render(const float time_now)
{
	const surface_t bg_surf = sprite_get_pixels(title_spr_gradient);

	rdpq_clear(RGBA16(0, 0, 0, 1));

	rdpq_set_mode_standard();
	rdpq_mode_dithering(DITHER_BAYER_NONE);
	rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);
	rdpq_tex_upload(TILE0, &bg_surf,
			&(const rdpq_texparms_t){
				.s.repeats = REPEAT_INFINITE,
				.s.mirror = MIRROR_REPEAT,
			});
	rdpq_texture_rectangle(TILE0, 0, 0, DSP_WID, DSP_HEI, time_now * 512,
			       0);
	const float traces_fade = (cosf(time_now * 3.14159f) + 1.0f) * 0.5f;
	const color_t traces_color =
		RGBA16(t3d_lerp(0.588f, 0.878f, traces_fade) * 31,
		       t3d_lerp(0.263f, 0.431f, traces_fade) * 31,
		       t3d_lerp(0.094f, 0.212f, traces_fade) * 31, 0x1);

	rdpq_mode_blender(
		RDPQ_BLENDER((IN_RGB, IN_ALPHA, MEMORY_RGB, INV_MUX_ALPHA)));
	rdpq_set_prim_color(traces_color);

	for (int i = 0; i < 3 * 3; i++) {
		int x = title_bg_pos[0] + 256 * (i % 3);
		int y = title_bg_pos[1] + 256 * (i / 3);

		if (x >= DSP_WID || y >= DSP_HEI)
			continue;

		rdpq_sprite_blit(title_spr_traces, x, y, NULL);
	}
}

void title_logo_render(void)
{
	rdpq_set_mode_standard();
	rdpq_mode_blender(
		RDPQ_BLENDER((IN_RGB, IN_ALPHA, MEMORY_RGB, INV_MUX_ALPHA)));
	rdpq_sprite_blit(title_spr_logo, 60, 25, NULL);
}

void title_credits_render(void)
{
	rdpq_set_mode_standard();
	font_printf(0, DSP_HEI - 42,
		    &(const rdpq_textparms_t){
			    .align = ALIGN_CENTER,
			    .width = DSP_WID,
		    },
		    "A Homebrew N64 Original by\n"
		    "Aeryk Ressler & Rosie Sapphire");
}
