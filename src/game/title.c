#include "util.h"
#include "vec2.h"

#include "engine/fade.h"

#include "game/scene_index.h"
#include "game/title.h"

/* sprites */
sprite_t *title_spr_logo;
sprite_t *title_spr_traces;
sprite_t *title_spr_gradient;

/* variables */
vec2i title_bg_pos;
int is_exiting;

static void title_terminate(UNUSED void *dummy)
{
	/* sprites */
	sprite_free(title_spr_gradient);
	sprite_free(title_spr_traces);
	sprite_free(title_spr_logo);
}

void title_init(void)
{
	fade_state_setup(FADE_STATE_IN);

	/* sprites */
	title_spr_logo = sprite_load("rom:/title_logo.ci8.sprite");
	title_spr_traces = sprite_load("rom:/title_traces.ia4.sprite");
	title_spr_gradient = sprite_load("rom:/title_gradient.rgba16.sprite");

	/* variables */
	title_bg_pos[0] = 0;
	title_bg_pos[1] = 0;
	is_exiting = 0;
}

int title_update(const joypad_buttons_t pressed,
		 UNUSED const joypad_inputs_t held)
{
	/* bg moving */
	title_bg_pos[0]--;
	if (title_bg_pos[0] <= -256)
		title_bg_pos[0] += 256;

	title_bg_pos[1]--;
	if (title_bg_pos[1] <= -256)
		title_bg_pos[1] += 256;

	const int cond_to_exit = pressed.start & ~is_exiting;

	is_exiting ^= cond_to_exit;
	if (fade_update(cond_to_exit) && is_exiting) {
		rdpq_call_deferred(title_terminate, NULL);
		return (SCENE_TESTAREA);
	}

	return (SCENE_TITLE);
}
