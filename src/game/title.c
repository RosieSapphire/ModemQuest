#include <libdragon.h>

#include "engine/fade.h"

#include "game/title.h"

/* sprites */
sprite_t *title_spr_logo;
sprite_t *title_spr_traces;
sprite_t *title_spr_gradient;

/* variables */
int title_bg_pos[2];

void title_init(void)
{
	/* sprites */
	title_spr_logo = sprite_load("rom:/title_logo.ci8.sprite");
	title_spr_traces = sprite_load("rom:/title_traces.ia4.sprite");
	title_spr_gradient = sprite_load("rom:/title_gradient.rgba16.sprite");

	/* variables */
	title_bg_pos[0] = 0;
	title_bg_pos[1] = 0;

	fade_init_vals(FADE_STATE_IN, 0);
}

void title_update(void)
{
	joypad_buttons_t pressed;

	joypad_poll();
	pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);

	/* bg moving */
	title_bg_pos[0]--;
	if (title_bg_pos[0] <= -256)
		title_bg_pos[0] += 256;

	title_bg_pos[1]--;
	if (title_bg_pos[1] <= -256)
		title_bg_pos[1] += 256;

	fade_update(FADE_PIXELS_PER_FRAME_DEFAULT, pressed.start);
}

void title_terminate(void)
{
	/* sprites */
	sprite_free(title_spr_gradient);
	sprite_free(title_spr_traces);
	sprite_free(title_spr_logo);
}
