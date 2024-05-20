#include <libdragon.h>

#include "engine/n64.h"
#include "engine/util.h"
#include "engine/config.h"

#include "game/title.h"

/* sprites */
static sprite_t *spr_logo;
static sprite_t *spr_traces;

/* variables */
static int bg_pos[2];

void title_init(void)
{
	/* sprites */
	spr_logo = sprite_load("rom:/title_logo.ci4.sprite");
	spr_traces = sprite_load("rom:/title_traces.ia4.sprite");

	/* variables */
	bg_pos[0] = 0;
	bg_pos[1] = 0;
}

void title_update(void)
{
	bg_pos[0]--;
	if (bg_pos[0] <= -256)
		bg_pos[0] += 256;

	bg_pos[1]--;
	if (bg_pos[1] <= -256)
		bg_pos[1] += 256;
}

void title_render(void)
{
	/* background */
	const float traces_fade =
		(cosf(((float)n64_ticks_now / TICKS_PER_SECOND) *
		      3.14159f) + 1.0f) * 0.5f;
	const color_t traces_color =
		RGBA16(
			lerpf(0.588f, 0.878f, traces_fade) * 31,
			lerpf(0.263f, 0.431f, traces_fade) * 31,
			lerpf(0.094f, 0.212f, traces_fade) * 31, 0x1
		);

	rdpq_set_mode_standard();
	rdpq_mode_alphacompare(1);
	rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);
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
	rdpq_sprite_blit(spr_logo, 58, 23, NULL);
}

void title_terminate(void)
{
	/* sprites */
	sprite_free(spr_logo);
	sprite_free(spr_traces);
}
