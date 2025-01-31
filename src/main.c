#include <libdragon.h>

#include "config.h"
#include "input.h"
#include "font.h"

#include "game/scene.h"
#include "game/fade_transition.h"
#include "game/title.h"
#include "game/testarea.h"

static float time_accumulated = 0.f;
static int dfs_handle = 0;

static void _init(void);
static void _update(const float dt);
static void _render(void);
static void _terminate(void);

static void (*scene_init_funcs[NUM_SCENE_INDICES])(void) = { title_init,
							     testarea_init };
static scene_index_t (*scene_update_funcs[NUM_SCENE_INDICES])(const float) = {
	title_update, testarea_update
};
static void (*scene_render_funcs[NUM_SCENE_INDICES])(const float) = {
	title_render, testarea_render
};
static void (*scene_terminate_funcs[NUM_SCENE_INDICES])(void) = {
	title_terminate, testarea_terminate
};

int main(void)
{
	_init();

	for (;;) {
		const float dt = DELTA_TIME;
		float frame_time = display_get_delta_time();

		time_accumulated += frame_time;
		while (time_accumulated >= dt) {
			_update(dt);
			time_accumulated -= dt;
		}
		_render();
	}

	_terminate();

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

static void _update(const float dt)
{
	input_poll();

	/* update active scene */
	scene_index_t scene_index_old = scene_index;
	assertf(scene_index < NUM_SCENE_INDICES,
		"Scene index %d is invalid. Could not update\n", scene_index);
	scene_index = (*scene_update_funcs[scene_index])(dt);

	/* check if we changed scenes and act accordingly */
	if (scene_index_old ^ scene_index) {
		(*scene_terminate_funcs[scene_index_old])();
		(*scene_init_funcs[scene_index])();
	}
}

static void _render(void)
{
	float subtick = time_accumulated / DELTA_TIME;

	rdpq_attach(display_get(), NULL);

	/* render active scene */
	assertf(scene_index < NUM_SCENE_INDICES,
		"Scene index %d is invalid. Could not render\n", scene_index);
	(*scene_render_funcs[scene_index])(subtick);

	rdpq_detach_show();
}

static void _terminate(void)
{
	fade_transition_terminate();
	font_terminate();
	input_terminate();

	/* terminate active scene */
	assertf(scene_index < NUM_SCENE_INDICES,
		"Scene index %d is invalid. Could not terminate\n",
		scene_index);
	(*scene_terminate_funcs[scene_index])();

	dfs_close(dfs_handle);

#ifdef DEBUG_ENABLED
	rdpq_debug_stop();
#endif

	rdpq_close();
	display_close();
}
