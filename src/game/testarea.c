#include "engine/fade.h"
#include "engine/config.h"
#include "engine/util.h"

#include "game/scene_index.h"
#include "game/player.h"
#include "game/tiles.h"
#include "game/testarea.h"

static int is_exiting;

static void testarea_terminate(void *dummy)
{}

void testarea_init(void)
{
	int px, py;
	tiles_init("rom:/testarea.map", &px, &py);
	player_init(px, py);
	fade_state_setup(FADE_STATE_DISABLED);

	is_exiting = 0;
}

int testarea_update(const joypad_buttons_t pressed, const joypad_inputs_t held)
{
	player_update(held);

	int exit_cond = pressed.start & ~is_exiting;

	is_exiting ^= exit_cond;
	if (fade_update(exit_cond))
	{
		rdpq_call_deferred(testarea_terminate, NULL);
		return (SCENE_TITLE);
	}

	return (SCENE_TESTAREA);
}

void testarea_render(void)
{
	rdpq_clear(RGBA16(0x4, 0x0, 0x7, 0x1));
	tiles_render();
	player_render();
	fade_render();
}
