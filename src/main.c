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

static int (*update_funcs[SCENE_CNT])(const joypad_buttons_t) = {
	title_update, testarea_update,
};

static void (*render_funcs[SCENE_CNT])(void) = {
	title_render, testarea_render,
};

int main(void)
{
#if DEBUG
	debug_initialize();
#endif
	n64_init();
	font_init();
	fade_load_spr();
	init_funcs[scene_index]();

	for (;;)
	{
		int skip_draw = 0;
		joypad_buttons_t pressed;

		n64_update_ticks_accum();
		while (n64_ticks_accum >= DELTATICKS)
		{
			const int scene_index_last = scene_index;

			joypad_poll();
			pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);

			scene_index = update_funcs[scene_index](pressed);
			if (scene_index ^ scene_index_last)
			{
				skip_draw = 1;
				n64_ticks_accum = 0;
				debugf("SKIPPING DRAW\n");
				break;
			}
			n64_ticks_accum -= DELTATICKS;
		}

		if (skip_draw)
		{
			rspq_wait();
			continue;
		}

		rdpq_attach(display_get(), NULL);
		render_funcs[scene_index]();
		rdpq_detach_show();
	}
}
