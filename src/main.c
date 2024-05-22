#include <libdragon.h>

#include "debug/debug.h"

#include "engine/config.h"
#include "engine/n64.h"
#include "engine/font.h"
#include "engine/fade.h"

#include "game/scene_index.h"
#include "game/title.h"

static void (*init_funcs[SCENE_CNT])(void) = {
	title_init, NULL,
};

static void (*update_funcs[SCENE_CNT])(void) = {
	title_update, NULL,
};

static void (*render_funcs[SCENE_CNT])(void) = {
	title_render, NULL,
};

static void (*terminate_funcs[SCENE_CNT])(void) = {
	title_terminate, NULL,
};

int main(void)
{
#if DEBUG
	debug_initialize();
#endif
	n64_init();
	font_init();
	fade_init_spr();
	init_funcs[scene_index]();

	for (;;)
	{
		n64_update_ticks_accum();

		while (n64_ticks_accum >= DELTATICKS)
		{
			update_funcs[scene_index]();
			n64_ticks_accum -= DELTATICKS;
		}

		rdpq_attach(display_get(), NULL);
		render_funcs[scene_index]();
		rdpq_detach_show();
	}

	terminate_funcs[scene_index]();
	fade_terminate_spr();
	font_terminate();
	n64_terminate();
}
