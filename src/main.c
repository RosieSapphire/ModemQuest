#include <libdragon.h>

#include "debug/debug.h"

#include "engine/config.h"
#include "engine/n64.h"
#include "engine/font.h"
#include "engine/fade.h"

#include "game/scene_index.h"
#include "game/title.h"
#include "game/testarea.h"

static void (*init_funcs[SCENE_CNT])(void) = {
	title_init, testarea_init,
};

static void (*render_funcs[SCENE_CNT])(void) = {
	title_render, testarea_render,
};

static int (*update_funcs[SCENE_CNT])(const joypad_buttons_t,
				      const joypad_inputs_t) = {
	title_update, testarea_update,
};

int main(void)
{
	srand(TICKS_READ());

#if DEBUG
	debug_initialize();
#endif
	n64_init();
	font_init();
	fade_load_spr();
	init_funcs[scene_index]();

	int scene_index_last = SCENE_INVALID;

	for (;;)
	{
		rdpq_attach(display_get(), NULL);
		render_funcs[scene_index]();
		rdpq_detach_show();

		n64_update_ticks_accum();
		while (n64_ticks_accum >= DELTATICKS)
		{
			joypad_buttons_t pressed;
			joypad_inputs_t held;

			joypad_poll();
			pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
			held = joypad_get_inputs(JOYPAD_PORT_1);

			scene_index_last = scene_index;
			scene_index = update_funcs[scene_index](pressed, held);
			if (scene_index_last ^ scene_index)
			{
				init_funcs[scene_index]();
				n64_ticks_accum = 0;
				rspq_wait();
				break;
			}

			n64_ticks_accum -= DELTATICKS;
		}
	}

	return (0);
}
