#include <libdragon.h>

#include "types.h"
#include "config.h"
#include "input.h"
#include "font.h"

#include "game/scene.h"
#include "game/fade_transition.h"
#include "game/title.h"
#include "game/testarea.h"

static f32 time_accumulated = 0.f;
static int dfs_handle = 0;

static void _init(void);
static void _update(const f32 dt);
static void _render(void);
static void _free(void);

static void (*scene_init_funcs[NUM_SCENE_INDICES])(void) = { title_init,
							     testarea_init };
static scene_index_t (*scene_update_funcs[NUM_SCENE_INDICES])(const f32) = {
	title_update, testarea_update
};
static void (*scene_render_funcs[NUM_SCENE_INDICES])(const f32) = {
	title_render, testarea_render
};
static void (*scene_free_funcs[NUM_SCENE_INDICES])(void) = { title_free,
							     testarea_free };

int main(void)
{
	_init();

	for (;;) {
		const f32 dt = DELTA_TIME;
		f32 frame_time = display_get_delta_time();

		time_accumulated += frame_time;
		while (time_accumulated >= dt) {
			_update(dt);
			time_accumulated -= dt;
		}
		_render();
	}

	_free();

	return 0;
}

static void _init(void)
{
	display_init(DISPLAY_RESOLUTION, DISPLAY_BITDEPTH, DISPLAY_NUM_BUFFERS,
		     DISPLAY_GAMMA, DISPLAY_FILTERS);
	rdpq_init();

#ifdef DEBUG_ENABLED
	debug_init_isviewer();
	debug_init_usblog();
	rdpq_debug_start();
#endif
	debugf("RUNNING\n");

	dfs_handle = dfs_init(DFS_DEFAULT_LOCATION);
	asset_init_compression(1);

	input_init();
	font_init();
	fade_transition_init();

	time_accumulated = 0.0f;

	/* initialize the active scene at beginning */
	assertf(scene_index < NUM_SCENE_INDICES,
		"Scene index %d is invalid. Could not initialize\n",
		scene_index);
	(*scene_init_funcs[scene_index])();
}

static void _update(const f32 dt)
{
	input_poll();

	/* update active scene */
	scene_index_t scene_index_old = scene_index;
	assertf(scene_index < NUM_SCENE_INDICES,
		"Scene index %d is invalid. Could not update\n", scene_index);
	scene_index = (*scene_update_funcs[scene_index])(dt);

	/* check if we changed scenes and act accordingly */
	if (scene_index_old ^ scene_index) {
		(*scene_free_funcs[scene_index_old])();
		(*scene_init_funcs[scene_index])();
	}
}

static void _render(void)
{
	f32 subtick = time_accumulated / DELTA_TIME;

	rdpq_attach(display_get(), NULL);

	/* render active scene */
	assertf(scene_index < NUM_SCENE_INDICES,
		"Scene index %d is invalid. Could not render\n", scene_index);
	(*scene_render_funcs[scene_index])(subtick);

	rdpq_detach_show();
}

static void _free(void)
{
	fade_transition_free();
	font_free();
	input_free();

	/* free active scene */
	assertf(scene_index < NUM_SCENE_INDICES,
		"Scene index %d is invalid. Could not free\n", scene_index);
	(*scene_free_funcs[scene_index])();

	dfs_close(dfs_handle);

#ifdef DEBUG_ENABLED
	rdpq_debug_stop();
#endif

	rdpq_close();
	display_close();
}
